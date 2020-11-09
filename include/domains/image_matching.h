#ifndef _IMAGE_MATCHING_H_
#define _IMAGE_MATCHING_H_

/*
    The image matching domain constructs a target image for an evolutionary procedure 
    to match. These target images are only binary matrices right now, this can be
    extended in the future.
*/

#include <domains/domain.h>
#include <phenotype/phenotype_specs/vector_phenotype_spec.h>
#include <util/vector_creation/vector_creation_policy.h>

namespace NeuroEvo {

template <typename G>
class ImageMatching : public Domain<G, bool>
{

public:

    ImageMatching(const std::shared_ptr<VectorCreationPolicy<bool>> vector_creation_policy,
                  const bool render = false, 
                  const bool domain_trace = false) :
        Domain<G, bool>(domain_trace, 1., std::nullopt, render),
        _vector_creation_policy(vector_creation_policy),
        _image_width(sqrt(vector_creation_policy->get_vector_size())),
        _target_image(create_target_image(0)) {}

private:

    const Matrix<bool> create_target_image(const unsigned run_num) const
    {
        const std::vector<bool> image_vec = _vector_creation_policy->generate_vector(run_num);
        return Matrix<bool>(_image_width, _image_width, image_vec);
    }

    void render() override 
    {
#if SFML_FOUND

        const std::vector<sf::RectangleShape> render_squares = build_render_squares();

        /* Define text */
        sf::Font font;
        std::stringstream font_path;
        font_path << NEURO_EVO_CMAKE_SRC_DIR << "/res/fonts/arial.ttf"; 
        font.loadFromFile(font_path.str());

        sf::Text target_text;
        target_text.setFont(font);
        target_text.setString("Target");
        target_text.setCharacterSize(24);
        target_text.setFillColor(sf::Color::Black);

        const float target_text_x = this->_window.getSize().x / 4. - 
                                    target_text.getGlobalBounds().width / 2.;
        const float text_y = 800;
        target_text.setPosition(sf::Vector2f(target_text_x, text_y));

        sf::Text org_text;
        org_text.setFont(font);
        org_text.setString("Organism guess");
        org_text.setCharacterSize(24);
        org_text.setFillColor(sf::Color::Black);

        const float org_text_x = this->_window.getSize().x * 3. / 4. - 
                                 org_text.getGlobalBounds().width / 2.;
        org_text.setPosition(sf::Vector2f(org_text_x, text_y));

        while(this->_window.isOpen())
        {

            sf::Event event;
            while(this->_window.pollEvent(event))
                if(event.type == sf::Event::Closed)
                    this->_window.close();

            this->_window.clear(sf::Color(192, 192, 192));

            //Render target image
            for(const auto& square : render_squares)
                this->_window.draw(square);

            //Render text
            this->_window.draw(target_text);
            if(_org_image.has_value())
                this->_window.draw(org_text);

            this->_window.display();
            
        }

#endif
    }

    double single_run(Organism<G, bool>& org, unsigned rand_seed) override 
    {
        const std::vector<bool> pheno_out = org.get_phenotype().activate();
        _org_image.emplace(_image_width, _image_width, pheno_out);

        if(!_target_image.has_value())
        {
            std::cerr << "Target image has not yet been set!" << std::endl;
            exit(0);
        }

        const double fitness = calculate_fitness();

        if(this->_render)
            render();

        return fitness;
    }

    double calculate_fitness() const
    {
        double fitness = 0.;

        //Compare organism guess to target image
        for(unsigned i = 0; i < _org_image->get_height(); i++)
            for(unsigned j = 0; j < _org_image->get_width(); j++)
                if(_org_image->at(i, j) == _target_image->at(i, j))
                    fitness += 1.;

        return (fitness / (_image_width * _image_width));

    }

#if SFML_FOUND
    std::vector<sf::RectangleShape> build_render_squares() const
    {

        const float square_width = 0.4 * this->_window.getSize().x / _image_width;
        const float y_pos_offset = this->_window.getSize().y / 2. -
                                   _image_width * square_width / 2.;
        const float target_x_pos_offset = this->_window.getSize().x / 4. -
                                          _image_width * square_width / 2.; 

        std::vector<sf::RectangleShape> squares(_image_width * _image_width * 2);

        for(std::size_t i = 0; i < _image_width; i++)
        {
            const float pos_y = y_pos_offset + i * square_width; 
            for(std::size_t j = 0; j < _image_width; j++)
            {
                const std::size_t square_index = i * _image_width + j;
                squares[square_index].setSize(sf::Vector2f(square_width, square_width));
                squares[square_index].setFillColor(_target_image->at(i, j) ? 
                                                   sf::Color::Black : sf::Color::White);
                const float pos_x = target_x_pos_offset + j * square_width;
                squares[square_index].setPosition(sf::Vector2f(pos_x, pos_y));
            }
        }

    if(_org_image.has_value())
    {
        const float org_x_pos_offset = this->_window.getSize().x * 3. / 4. -
                                       _image_width * square_width / 2.; 
        for(std::size_t i = 0; i < _image_width; i++)
        {
            const float pos_y = y_pos_offset + i * square_width; 
            for(std::size_t j = 0; j < _image_width; j++)
            {
                const std::size_t square_index = i * _image_width + j + 
                                                 _image_width * _image_width;
                squares[square_index].setSize(sf::Vector2f(square_width, square_width));
                squares[square_index].setFillColor(_org_image->at(i, j) ? 
                                                   sf::Color::Black : sf::Color::White);
                const float pos_x = org_x_pos_offset + j * square_width;
                squares[square_index].setPosition(sf::Vector2f(pos_x, pos_y));
            }
        }
    }

        return squares;

    }
#endif

    bool check_phenotype_spec(const PhenotypeSpec& pheno_spec) const override
    {
         auto vec_pheno_spec = dynamic_cast<const VectorPhenotypeSpec*>(&pheno_spec);

        if(vec_pheno_spec == nullptr)
        {
            std::cerr << "Only vector phenotype specifications are allowed" <<
                " for the Concentric Squares domain!" << std::endl;
            return false;
        }

        if(vec_pheno_spec->get_num_params() != (_image_width * _image_width))
        {
            std::cerr << "The number of phenotype parameters does not match the size of the" <<
                " target image" << std::endl;
            std::cerr << "Num pheno params: " << vec_pheno_spec->get_num_params() << std::endl;
            std::cerr << "Image size: " << (_image_width * _image_width) << std::endl;
            return false;
        }

        return true;
    }

    void trial_reset(const unsigned trial_num) override {}
    void exp_run_reset_impl(const unsigned run_num) override 
    {
        mtx.lock();

        _target_image = create_target_image(run_num);

        /*
        this->set_render(true);
        this->render();
        this->set_render(false);
        */

        mtx.unlock();
    }

    ImageMatching* clone_impl() const override
    {
        return new ImageMatching(*this);
    }

    std::shared_ptr<VectorCreationPolicy<bool>> _vector_creation_policy;
    const unsigned _image_width;
    std::optional<Matrix<bool>> _target_image;
    std::optional<Matrix<bool>> _org_image;
};

} // namespace NeuroEvo

#endif
