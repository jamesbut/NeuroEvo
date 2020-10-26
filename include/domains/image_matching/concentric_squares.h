#ifndef _CONCENTRIC_SQUARES_H_
#define _CONCENTRIC_SQUARES_H_

/*
    Concentric squares is a domain where the aim is to match
    a target image of different coloured squares. The domain
    is described in Koutnis, Clune et al 2016.
*/

#include <domains/domain.h>
#include <phenotype/phenotype_specs/vector_phenotype_spec.h>
#include <thread>

namespace NeuroEvo {

template <typename G>
class ConcentricSquares : public Domain<G, bool>
{

public: 

    ConcentricSquares(const unsigned image_width,
                      const bool render = false, const bool domain_trace = false) :
        Domain<G, bool>(domain_trace, calculate_completion_fitness(image_width), 
                        std::nullopt, render),
        _image_width(image_width),
        _target_image(create_target_image()) {}

private:

    double single_run(Organism<G, bool>& org, unsigned rand_seed) override 
    {

        const std::vector<bool> pheno_out = org.get_phenotype().activate();
        _org_image.emplace(_image_width, _image_width, pheno_out);

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
                if(_org_image->at(i, j) == _target_image.at(i, j))
                    fitness += 1.;

        return fitness;

    }

    const Matrix<bool> create_target_image() const
    {
        //Sets whether the outer square is 1 or 0
        const bool outer_square = true;

        //The algorithm to create the target image is somewhat complicated to index - I do 
        //apoligise. Maybe there is an easier way to do it but this is what I figured out
        //for now
        Matrix<bool> target_image(_image_width, _image_width);
        target_image.fill(!outer_square);

        //Upper half
        for(unsigned i = 0; i < (target_image.get_height() / 2 + 1); i++)
        {
            //Alternating part  
            for(unsigned j = 0; j < i; j++)
                if((j % 2) == 0)
                    target_image.set(i, j, outer_square);

            //Const part
            if((i % 2) == 0)
                for(unsigned j = 0; j < (target_image.get_width() - 2 * i); j++)
                    target_image.set(i, j + i, outer_square);

            //Alternating part
            for(unsigned j = (target_image.get_width() - i); j < target_image.get_width(); j++)
                if((j % 2) == static_cast<int>(target_image.get_width() % 2 == 0))
                    target_image.set(i, j, outer_square);

        }

        //Lower half
        for(unsigned i = (target_image.get_height() / 2 + 1); i < target_image.get_height(); i++)
        {
            //Alternating part  
            for(unsigned j = 0; j < (target_image.get_width() - i); j++)
                if((j % 2) == 0)
                    target_image.set(i, j, outer_square);

            //Const part
            if((i % 2) == static_cast<int>((target_image.get_width() % 2) == 0))
            {
                const unsigned num_consts = 2 * i - target_image.get_width() + 2;
                for(unsigned j = 0; j < num_consts; j++)
                {
                    const unsigned j_index = target_image.get_width() - i - 1 + j;
                    target_image.set(i, j_index, outer_square);
                }
            }

            //Alternating part
            for(unsigned j = i; j < target_image.get_width(); j++)
                if((j % 2) == static_cast<int>(target_image.get_width() % 2 == 0))
                    target_image.set(i, j, outer_square);

        }

        //std::cout << target_image << std::endl;

        return target_image;
    }

    double calculate_completion_fitness(const unsigned image_width) const
    {
        return static_cast<double>(image_width * image_width);
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
            this->_window.draw(org_text);

            this->_window.display();
            
        }

#endif
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
                squares[square_index].setFillColor(_target_image.at(i, j) ? 
                                                   sf::Color::Black : sf::Color::White);
                const float pos_x = target_x_pos_offset + j * square_width;
                squares[square_index].setPosition(sf::Vector2f(pos_x, pos_y));
            }
        }

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

        return squares;

    }
#endif

    void exp_run_reset_impl() override {}
    void trial_reset(const unsigned trial_num) override {}

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
            return false;
        }

        return true;
    }

    ConcentricSquares<G>* clone_impl() const override
    {
        return new ConcentricSquares<G>(*this);
    }

    const unsigned _image_width;
    const Matrix<bool> _target_image;

    std::optional<Matrix<bool>> _org_image;

};

} // namespace NeuroEvo

#endif
