#include <util/models/supervised_model.h>
#include <util/formatting.h>
#include <util/torch/schedulers/step_lr.h>
#include <util/torch/schedulers/reduce_lr_on_plateau.h>
#include <util/torch/schedulers/per_epoch_lr.h>
#include <util/torch/schedulers/step_lr.h>

#include <torch/optim/schedulers/step_lr.h>

namespace NeuroEvo {

SupervisedFeedForward::SupervisedFeedForward(NetworkBuilder& net_builder,
                                             const torch::Tensor& training_data,
                                             const torch::Tensor& training_labels,
                                             const std::optional<const torch::Tensor>&
                                                 test_data,
                                             const std::optional<const torch::Tensor>&
                                                 test_labels) :
    TrainableModel(training_data, test_data, net_builder, "ie_supervised.pt"),
    _training_labels(training_labels),
    _test_labels(test_labels) {}

void SupervisedFeedForward::train(const unsigned num_epochs, const unsigned batch_size,
                                  const double weight_decay, const bool trace,
                                  const unsigned test_every)
{

    const double learning_rate = 1e-4;
    torch::optim::Adam optimizer(
        _model->parameters(),
        torch::optim::AdamOptions(learning_rate).weight_decay(weight_decay)
    );

    /*
    const double momentum = 0.99;
    const bool nesterov = true;
    torch::optim::SGD optimizer(
        _model->parameters(),
        torch::optim::SGDOptions(learning_rate).momentum(momentum).nesterov(nesterov)
    );

    torch::optim::RMSprop optimizer(
        _model->parameters(),
        torch::optim::RMSpropOptions(learning_rate)
    );
    */

    //Learning rate scheduler
    unsigned epoch_num;
    /*
    const unsigned step_size = 250;
    const double gamma = 0.5;
    StepLR lr_scheduler = StepLR(optimizer, epoch_num, step_size, gamma);
    */

    double avg_loss_dbl = 0;

    /*
    const double factor = 0.5;
    const unsigned patience = 150;
    //const std::optional<double> min_lr = 1e-6;
    const std::optional<double> min_lr = std::nullopt;
    ReduceLROnPlateau lr_scheduler = ReduceLROnPlateau(optimizer, avg_loss_dbl,
                                                       factor, patience, min_lr);
    */

    const unsigned step_size = 250;
    const double gamma = 0.5;
    torch::optim::StepLR lr_scheduler(optimizer, step_size, gamma);
    //StepLR lr_scheduler(optimizer, epoch_num, step_size, gamma);

    /*
    std::map<unsigned, double> epoch_lr_map;
    //1024 nodes
    //epoch_lr_map[200] = 3e-5;
    //epoch_lr_map[400] = 1e-5;
    //4096 nodes
    epoch_lr_map[100] = 2e-5;
    epoch_lr_map[200] = 5e-6;
    epoch_lr_map[300] = 2e-6;
    epoch_lr_map[400] = 1e-6;
    PerEpochLR lr_scheduler = PerEpochLR(optimizer, epoch_num, epoch_lr_map);
    */

    torch::Tensor avg_test_loss = torch::zeros({1}, {torch::kFloat64});

    const unsigned column_width = 16;
    const std::vector<std::string> header_data{"Epoch",
                                               "Training loss",
                                               "Test loss"};
    print_table_row(header_data, column_width, true, true);

    for(epoch_num = 0; epoch_num < num_epochs; epoch_num++)
    {

        //Dump decoder
        if(epoch_num % test_every == 0)
            write_model(epoch_num);

        const std::vector<std::pair<torch::Tensor, torch::Tensor>> batches =
            generate_batches(batch_size, _training_data, _training_labels);

        torch::Tensor total_loss = torch::zeros(1, {torch::kFloat64});

        for(const auto& batch : batches)
        {
            _model->zero_grad();
            const torch::Tensor output = _model->forward(batch.first);

            torch::Tensor loss = loss_function(output, batch.second);

            loss.backward();
            total_loss += loss;

            optimizer.step();

        }


        torch::Tensor avg_loss = total_loss / _training_data.size(0);
        avg_loss_dbl = avg_loss.item<double>();

        // Test on test set
        if(((epoch_num+1) % test_every == 0) && _test_data.has_value())
        {
            const torch::Tensor test_output = _model->forward(_test_data.value());
            const torch::Tensor test_loss = loss_function(test_output, _test_labels.value());
            avg_test_loss = test_loss / _test_data->size(0);
        }

        const std::vector<double> row_data{static_cast<double>(epoch_num),
                                           avg_loss.item<double>(),
                                           avg_test_loss.item<double>()};
        print_table_row(row_data, column_width);

        //lr_scheduler.step();

    }

}

torch::Tensor SupervisedFeedForward::loss_function(const torch::Tensor& output,
                                                   const torch::Tensor& labels) const
{

    /*
    torch::Tensor loss = torch::nn::functional::binary_cross_entropy(
        output,
        labels,
        torch::nn::functional::BinaryCrossEntropyFuncOptions().reduction(torch::kSum)
    );
    */
    torch::Tensor loss = torch::nn::functional::mse_loss(
        output,
        labels,
        torch::nn::functional::MSELossFuncOptions().reduction(torch::kSum)
    );

    return loss;

}

} // namspace NeuroEvo
