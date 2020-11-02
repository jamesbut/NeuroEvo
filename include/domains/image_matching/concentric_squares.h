#ifndef _CONCENTRIC_SQUARES_H_
#define _CONCENTRIC_SQUARES_H_

/*
    Concentric squares is a domain where the aim is to match
    a target image of different coloured squares. The domain
    is described in Koutnis, Clune et al 2016.
*/

#include <domains/image_matching/image_matching.h>

namespace NeuroEvo {

template <typename G>
class ConcentricSquares : public ImageMatching<G>
{

public: 

    ConcentricSquares(const unsigned image_width,
                      const bool render = false, const bool domain_trace = false) :
        ImageMatching<G>(image_width, render, domain_trace), 
        _leading_square(true)
    {
        this->_target_image = create_target_image();
    }

private:

    const Matrix<bool> create_target_image() const override
    {

        //The algorithm to create the target image is somewhat complicated to index - I do 
        //apologise. Maybe there is an easier way to do it but this is what I figured out
        //for now
        Matrix<bool> target_image(this->_image_width, this->_image_width);
        target_image.fill(!_leading_square);

        //Upper half
        for(unsigned i = 0; i < (target_image.get_height() / 2 + 1); i++)
        {
            //Alternating part  
            for(unsigned j = 0; j < i; j++)
                if((j % 2) == 0)
                    target_image.set(i, j, _leading_square);

            //Const part
            if((i % 2) == 0)
                for(unsigned j = 0; j < (target_image.get_width() - 2 * i); j++)
                    target_image.set(i, j + i, _leading_square);

            //Alternating part
            for(unsigned j = (target_image.get_width() - i); j < target_image.get_width(); j++)
                if((j % 2) == static_cast<int>(target_image.get_width() % 2 == 0))
                    target_image.set(i, j, _leading_square);

        }

        //Lower half
        const unsigned i_start = target_image.get_height() / 2 + 1;
        for(unsigned i = i_start; i < target_image.get_height(); i++)
        {
            //Alternating part  
            for(unsigned j = 0; j < (target_image.get_width() - i); j++)
                if((j % 2) == 0)
                    target_image.set(i, j, _leading_square);

            //Const part
            if((i % 2) == static_cast<int>((target_image.get_width() % 2) == 0))
            {
                const unsigned num_consts = 2 * i - target_image.get_width() + 2;
                for(unsigned j = 0; j < num_consts; j++)
                {
                    const unsigned j_index = target_image.get_width() - i - 1 + j;
                    target_image.set(i, j_index, _leading_square);
                }
            }

            //Alternating part
            for(unsigned j = i; j < target_image.get_width(); j++)
                if((j % 2) == static_cast<int>(target_image.get_width() % 2 == 0))
                    target_image.set(i, j, _leading_square);

        }

        //std::cout << target_image << std::endl;

        return target_image;
    }

    void exp_run_reset_impl(const unsigned run_num) override 
    {
        if(run_num == 0)
            _leading_square = true;
        else 
            _leading_square = false;
        //One must regenerate the target image if the leading square is changed
        this->_target_image = create_target_image();
    }

    ConcentricSquares<G>* clone_impl() const override
    {
        return new ConcentricSquares<G>(*this);
    }

    //Sets whether the leading outer square is 1 or 0
    bool _leading_square;

};

} // namespace NeuroEvo

#endif
