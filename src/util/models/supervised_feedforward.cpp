#include <util/models/supervised_feedforward.h>
#include <util/torch_utils.h>
#include <util/formatting.h>

namespace NeuroEvo {

SupervisedFeedForward::SupervisedFeedForward(NetworkBuilder& net_builder,
                                             const torch::Tensor& training_data,
                                             const torch::Tensor& training_labels,
                                             const std::optional<const torch::Tensor>& 
                                                 test_data,
                                             const std::optional<const torch::Tensor>& 
                                                 test_labels) :
    TrainableModel(training_data, test_data),
    _net(build_torch_network(net_builder, nullptr)),
    _training_labels(training_labels),
    _test_labels(test_labels) {}

void SupervisedFeedForward::train(const unsigned num_epochs, const unsigned batch_size,
                                  const double weight_decay, const bool trace,
                                  const unsigned test_every)
{
    
    const double learning_rate = 1e-3;
    auto adam_options = torch::optim::AdamOptions(learning_rate);
    adam_options.weight_decay(weight_decay);
    torch::optim::Adam optimizer(
        _net->parameters(), 
        adam_options
    );

    torch::Tensor avg_test_loss = torch::zeros({1}, {torch::kFloat64});

    const unsigned column_width = 16;
    const std::vector<std::string> header_data{"Epoch",
                                               "Training loss",
                                               "Test loss"};
    print_table_row(header_data, column_width, true, true);

    for(unsigned i = 0; i < num_epochs; i++)
    {

        const std::vector<std::pair<torch::Tensor, torch::Tensor>> batches =
            generate_batches(batch_size, _training_data, _training_labels);

        torch::Tensor total_loss = torch::zeros(1, {torch::kFloat64});

        for(const auto& batch : batches)
        {
            _net->zero_grad();
            const torch::Tensor output = _net->forward(batch.first);

            torch::Tensor loss = loss_function(output, batch.second);

            loss.backward();
            total_loss += loss;

            optimizer.step();

        }

        torch::Tensor avg_loss = total_loss / _training_data.size(0);

        // Test on test set
        if(((i+1) % test_every == 0) && _test_data.has_value())
        {
            const torch::Tensor test_output = _net->forward(_test_data.value());
            const torch::Tensor test_loss = loss_function(test_output, _test_labels.value());
            avg_test_loss = test_loss / _test_data->size(0);
        }

        const std::vector<double> row_data{static_cast<double>(i),
                                           avg_loss.item<double>(),
                                           avg_test_loss.item<double>()};
        print_table_row(row_data, column_width);

    }

}

void SupervisedFeedForward::test(const torch::Tensor& test_data) const
{
    std::cout << "Test input: " << std::endl;
    std::cout << test_data << std::endl;
    const torch::Tensor test_output = _net->forward(test_data);
    std::cout << "Test output: " << std::endl;
    std::cout << test_output << std::endl;
}

torch::Tensor SupervisedFeedForward::loss_function(const torch::Tensor& output,
                                                   const torch::Tensor& labels) const
{

    torch::Tensor loss = torch::nn::functional::binary_cross_entropy(
        output, 
        labels,
        torch::nn::functional::BinaryCrossEntropyFuncOptions().reduction(torch::kSum)
    );

    return loss;

}

void SupervisedFeedForward::print_params() const
{
    std::cout << _net->parameters() << std::endl;
}

} // namspace NeuroEvo
