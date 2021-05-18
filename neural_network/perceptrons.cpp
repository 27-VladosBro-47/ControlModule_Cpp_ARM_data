#include "perceptrons.h"

    Perceptrons::Perceptrons(int numbOfPerceptrons, int numbOfWeights)
    {
        quantityOfPerceptrons = numbOfPerceptrons;
        quantityOfWeights = numbOfWeights;

        // Створення шару персептронів і вагів
        perceptronsLayer.resize(numbOfPerceptrons);
        for(auto& count : perceptronsLayer)
        {
            count.weights.resize(numbOfWeights);
            count.d_weights.resize(numbOfWeights);
        }

        // Здвиг
        bias.value=bias.d_value=bias.derivative = 1.0;
        bias.weights.resize(numbOfPerceptrons);
        bias.d_weights.resize(numbOfPerceptrons);

    }

    Perceptrons::~Perceptrons()
    {
        // delete perceptronLayer;
        // delete bias;
    }

    // // Отримання масиву персептронів, що належать одному слою
    // std::vector<perceptron>* Perceptrons::getPerceptronLayer()
    // {
    //     return perceptronLayer;
    // }
    // // Отримання персептрону здвига
    // perceptron*  Perceptrons::getBias()
    // {
    //     return bias;
    // }
    // Отримання кількості персептронів у слої
    int Perceptrons::getQuantityOfPerceptrons()
    {
        return perceptronsLayer.size();
    }
    // Отримання кількості ваг, що виходять від кожного персептрона в слою
    int Perceptrons::getQuantityOfWeights()
    {
        return quantityOfWeights;
    }