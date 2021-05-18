#ifndef PERCEPTRONS
#define PERCEPTRONS

#include <string>
#include <vector>

class Perceptrons
{
public:
    struct perceptron
    {
        double value;                   // x
        double d_value;                 // d_x
        std::vector<double> weights;
        std::vector<double> d_weights;
        double derivative;                    // z
    };
    
    std::vector<perceptron> perceptronsLayer; // perceptronsArray
    perceptron bias;

    int quantityOfPerceptrons;
    int quantityOfWeights;

    //==========Методи==========//

    Perceptrons(int numbOfPerceptrons, int numbOfWeights);

    ~Perceptrons();

    // // Отримання масиву персептронів, що належать одному слою
    // std::vector<perceptron>* getPerceptronLayer();
    // // Отримання персептрону здвига
    // perceptron*  getBias();
    // Отримання кількості персептронів у слої
    int getQuantityOfPerceptrons();
    // Отримання кількості ваг, що виходять від кожного персептрона в слою
    int getQuantityOfWeights();

};

#endif // PERCEPTRONS