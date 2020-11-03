#ifndef _PLACE_SQUARE_H_
#define _PLACE_SQUARE_H_

/*
    The place sqaure image matching creates a target image that consists of a placed 
    black square
*/

#include <domains/image_matching/image_matching.h>

namespace NeuroEvo {

template <typename G>
class PlaceSquare : public ImageMatching<G>
{

public:

    PlaceSquare(const unsigned image_width,
                const bool render = false, const bool domain_trace = false) :
        ImageMatching<G>(image_width, render, domain_trace),
        _square_pos(0, 0)
    {
        this->_target_image = create_target_image();
    }

private:

    const Matrix<bool> create_target_image() const override 
    {
        Matrix<bool> target_image(this->_image_width, this->_image_width);
        target_image.fill(false);

        std::cout << "x: " << _square_pos.first << " y: " << _square_pos.second << std::endl;

        target_image.set(_square_pos.second, _square_pos.first, true);

        return target_image;
    }

    std::pair<unsigned, unsigned> calculate_square_pos(unsigned run_num) const
    {
        //Leave out middle square
        if(run_num > 11)
            run_num += 1;
        const unsigned x_pos = run_num % this->_image_width;
        const unsigned y_pos = floor((double)run_num / (double)this->_image_width);
        return std::make_pair(x_pos, y_pos);
    }

    void exp_run_reset_impl(const unsigned run_num) override
    {

        _square_pos = calculate_square_pos(run_num);

        this->_target_image = create_target_image();
        this->set_render(true);
        this->render();
        this->set_render(false);
    }

    PlaceSquare<G>* clone_impl() const override
    {
        return new PlaceSquare<G>(*this);
    }

    //I use this pair as coordinates (x, y)
    std::pair<unsigned, unsigned> _square_pos;

};

} // namespace NeuroEvo

#endif
