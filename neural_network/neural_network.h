#ifndef NEURAL_NETWORK
#define NEURAL_NETWORK

#include "perceptrons.h"
#include "mediapipe/examples/desktop/controle_module/json_lib/json.hpp"

#include <iomanip>>
#include <fstream>
#include <ctime>
#include <cmath>
#include <string>
#include <vector>
#include <thread>

class NeuralNetwork
{
private:
    // Зберігає кількість слоїв у нейромережі
    int countLayers;
    // Масив вказівників на слої нейромережі
    std::vector<Perceptrons*> layers;

    bool isLearning;
    
public:

    // Буферна змінна, для зберігання значень вихідного шару
    std::vector<double> arr;

// inputPerceptrons - кількість персептронів у вхідному шарі
// outputPerceptrons - кількість персептронів у вихідному шарі
// hiddenLayers - вектор, що містить кількість персоптронів у прихованих шарах
// Кількість елементів hiddenLayers позначає кількість прихованих шарів
NeuralNetwork(int inputPerceptrons, int outputPerceptrons, std::vector<int> hiddenLayers)
{
    isLearning = 0;
    
    countLayers = 2 + hiddenLayers.size();

    layers.resize(countLayers);

    // Ініціалізація вектору шарів персептронів
    for(int numbLayers = 0; numbLayers < countLayers; numbLayers++)
    {
            if(numbLayers == 0)                     // Вхідний шар
            {
                layers[0] = new Perceptrons(inputPerceptrons, hiddenLayers[0]);
            } 
            else 
            {
                if(numbLayers == (countLayers-2))   // Останній прихований шар
                {
                    layers[numbLayers] = new Perceptrons(hiddenLayers[numbLayers-1], outputPerceptrons);
                } 
                else 
                {
                    if(numbLayers == (countLayers-1))   // Вихідний шар
                    {
                        layers[numbLayers-1] = new Perceptrons(outputPerceptrons, 1);
                    } 
                    else 
                    {                                   // Прихований шар
                        layers[numbLayers] = new Perceptrons(hiddenLayers[numbLayers-1], hiddenLayers[numbLayers]);
                    
                    }
                }
            }

    }

    arr.resize(outputPerceptrons);
}

~NeuralNetwork()
{
    for(int numbLayers = 0; numbLayers < countLayers; numbLayers++)
    {
        delete layers[numbLayers];
    }
}

// Функція встановлює значення на вхідному шарові персептрона
// vector<double> data - вектор, що містить значення вхідного слою персептронів
void setInputPerceptrons(std::string type, std::vector<double> &data)
{
    if(type == "hand")
    {
        for(int numbOfpoints = 0; numbOfpoints < data.size(); numbOfpoints++)
        {
            layers[0]->perceptronsLayer[numbOfpoints].value = data[numbOfpoints];
        }
    }
}

// Функція виставляє початкові значення для значень персептронів
void setPrimaryPerseptrons()
{
    srand(time(NULL));
    rand();

    for (int layer = 0; layer < countLayers; layer++)
    {
        for (int perceptron = 0; perceptron < layers[layer]->quantityOfPerceptrons; perceptron++)
        {
            layers[layer]->perceptronsLayer[perceptron].value = (double)(rand()) / RAND_MAX * (1.0 - 0.01) + 0.01;
        }

    }
}

// Функція виставляє початкові значення для ваг персептронів
void setPrimaryWeights()
{
    srand(time(NULL));
    rand();

    for (int layer = 0; layer < countLayers; layer++)
    {
        for (int perceptron = 0; perceptron < layers[layer]->quantityOfPerceptrons; perceptron++)
        {
            for (int weight = 0; weight < layers[layer]->quantityOfWeights; weight++)
            {
                layers[layer]->perceptronsLayer[perceptron].weights[weight] = (double)(rand()) / RAND_MAX * (1.0 - 0.01) + 0.01 - 0.5;
                layers[layer]->perceptronsLayer[perceptron].d_weights[weight] = 0.0;
            }

            layers[layer]->bias.weights[perceptron] = (double)(rand()) / RAND_MAX * (1.0 - 0.01) + 0.01 - 0.5;
        }

    }
}

// Сигмоїда
double sigm(double x)
{
    return (1.0 / (1.0 + exp(-x)));
}

// Похідна сигмоїди
double d_sigm(double x)
{
    return (sigm(x) * (1 - sigm(x)))+0.00001;
}

// Метод обраховує персептрони наступного слою   ---   Застосовані потоки
// begin i end - вказують діапазон персептронів, які необхідно обрахувати
// layer - номер слою, у якому обраховуються персептрони
void calculatePerceptrons(int begin, int end, int layer)
{
    for (int perceptron = begin; perceptron < end; perceptron++)
    {
        layers[layer]->perceptronsLayer[perceptron].derivative = 0;

        for (int perceptron1 = 0; perceptron1 < layers[layer - 1]->quantityOfPerceptrons; perceptron1++)
        {
            layers[layer]->perceptronsLayer[perceptron].derivative +=
                layers[layer - 1]->perceptronsLayer[perceptron1].value *
                layers[layer - 1]->perceptronsLayer[perceptron1].weights[perceptron];
        }

        layers[layer]->perceptronsLayer[perceptron].derivative += layers[layer]->bias.weights[perceptron];

        layers[layer]->perceptronsLayer[perceptron].value = sigm(layers[layer]->perceptronsLayer[perceptron].derivative);
    }
}

// Метод обраховує значення нейромережі
void calculateNeuralNetwork()
{
    for (int layer = 0; layer < (countLayers - 1); layer++)
    {
        // Отримую кількість персептронів, які необхідно обрахувати на наступному слої нейромережі
        int quantityPerceptrons = layers[layer + 1]->quantityOfPerceptrons;
        calculatePerceptrons(0, quantityPerceptrons, layer + 1);
    }

    for(int i = 0; i < arr.size(); i++)
    {
        arr[i] = layers[countLayers - 1]->perceptronsLayer[i].value;
    }
}

// Функція виставляє в останьому слої нейромережі в змінних d_x "правильні значення" персептронів
// Потім відбувається обрахування d_x = x - d_x
void setTrueAnswer(int numbPerceptron)
{

    for (int perceptron = 0; perceptron < layers[countLayers - 1]->quantityOfPerceptrons; perceptron++)
    {
        if (perceptron == numbPerceptron)
        {
            layers[countLayers - 1]->perceptronsLayer[perceptron].d_value = layers[countLayers - 1]->perceptronsLayer[perceptron].value - 1.0;
        }
        else
        {
            layers[countLayers - 1]->perceptronsLayer[perceptron].d_value = layers[countLayers - 1]->perceptronsLayer[perceptron].value - 0.0;
        }
    }

}


// --- з.р.п. - Зворотнє розповсюдження помилки ---

// Обчислення помилки для здвигу за методом з.р.п.
void backPropagationBias(int layer)
{
    for (int bias = 0; bias < layers[layer]->quantityOfPerceptrons ; bias++)
    {
        double d_weight = 0;

        double d_value = layers[layer]->perceptronsLayer[bias].d_value;
        double derivative = layers[layer]->perceptronsLayer[bias].derivative;

        d_weight = 2*(d_value)*d_sigm(derivative);

        layers[layer]->bias.d_weights[bias] = d_weight;
    }
}

// Обчислення помилки для ваг за методом з.р.п.
void backPropagationWeights(int layer)
{
    for (int perceptron = 0; perceptron < layers[layer - 1]->quantityOfPerceptrons; perceptron++)
    {
        for (int weight = 0; weight < layers[layer - 1]->quantityOfWeights; weight++)
        {
            double d_weight = 0.0;

            double d_value = layers[layer]->perceptronsLayer[weight].d_value;
            double derivative = layers[layer]->perceptronsLayer[weight].derivative;
            double x_value = layers[layer - 1]->perceptronsLayer[perceptron].value;

            d_weight = 2*(d_value)*d_sigm(derivative)*x_value;

            layers[layer - 1]->perceptronsLayer[perceptron].d_weights[weight] = d_weight;
        }

    }

    backPropagationBias(layer);
}

void weightsDifference(double coefficient = 0.02)
{
    for (int layer = countLayers - 1; layer > 0; --layer)
    {
        for (int perceptron = 0; perceptron < layers[layer - 1]->quantityOfPerceptrons; perceptron++)
        {
            for (int weight = 0; weight < layers[layer - 1]->quantityOfWeights; weight++)
            {

                layers[layer - 1]->perceptronsLayer[perceptron].weights[weight] -=
                    coefficient * layers[layer - 1]->perceptronsLayer[perceptron].d_weights[weight];
                layers[layer - 1]->perceptronsLayer[perceptron].d_weights[weight] = 0.0;

            }
        }

        for (int bias = 0; bias < layers[layer]->quantityOfPerceptrons; bias++)
        {
            layers[layer]->bias.weights[bias] -= coefficient * layers[layer]->bias.d_weights[bias];
            layers[layer]->bias.d_weights[bias] = 0.0;
        }
    }
}

void backPropagationPerceptrons(int layer)
{
    for (int perceptron = 0; perceptron < layers[layer - 1]->quantityOfPerceptrons; perceptron++)
    {
        double d_perceptron = 0;
        for (int weight = 0; weight < layers[layer - 1]->quantityOfWeights; weight++)
        {
            double d_value = layers[layer]->perceptronsLayer[weight].d_value;
            double derivative = layers[layer]->perceptronsLayer[weight].derivative;
            double w_weight = layers[layer - 1]->perceptronsLayer[perceptron].weights[weight];

            d_perceptron += 2*(d_value)*d_sigm(derivative)*w_weight;
        }

        layers[layer - 1]->perceptronsLayer[perceptron].d_value = d_perceptron;
        layers[layer - 1]->perceptronsLayer[perceptron].value -= layers[layer - 1]->perceptronsLayer[perceptron].d_value;

    }

}

// Реалізація методу з.р.п.
void backPropagation()
{
    for (int layer = countLayers - 1; layer > 0; --layer)
    {
        backPropagationWeights(layer);
        backPropagationPerceptrons(layer);
    }
    weightsDifference();
}

// Функція визначає рівень навчиності нейромережі
// Чим значення double, що повертає функція, ближче до нуля, тим більше навчина нейромережа
double levelOfEducation()
{
    double result = 0.0;
    for (int perceptron = 0; perceptron < layers[countLayers - 1]->quantityOfPerceptrons; perceptron++)
    {
        double d_value = layers[countLayers - 1]->perceptronsLayer[perceptron].d_value;

        result += pow((d_value), 2);
    }
    return result;
}

// --- з.р.п. - Зворотнє розповсюдження помилки ---

void learningProcess()
{   

}

bool saveWeights(std::string path)
{
    using json = nlohmann::json;

    std::ifstream in_file;
    std::ofstream out_file;
    json j;

    auto openFile = [&in_file](std::string fileName)
    {
    in_file.open(fileName);

        if(!in_file.is_open())
        {
            in_file.close();

            std::fstream f (fileName, std::ios::in | std::ios::out | std::ios::app);
            f << '{' << std::endl << std::endl << '}';
            f.close();

            in_file.open(fileName);
        }


    };

    auto saveAndCloseFile = [&](std::string fileName)
    {
        in_file.close();
        out_file.open(fileName);

        out_file << std::setw(4) << j << std::endl;
        out_file.close();
    };

    std::string fileName;

    fileName = path + "parameters.json";
    openFile(fileName);

    j = json::parse(in_file);

    j["Size"] = countLayers;
    for(int layer = 0; layer<countLayers; layer++)
    {
        j["perceprons"][layer] = layers[layer]->quantityOfPerceptrons;
    }

    saveAndCloseFile(fileName);
    j.clear();

        for(int layer = 0; layer<countLayers; layer++)
        {

            // Визначаю назву файла
            if(layer == countLayers-1)  { fileName = path +  "outputLayer_perceptronsWeights.json"; }         // Вихідний слой
            else
            {   if(layer == 0){ fileName = path +  "inputLayer_perceptronsWeights.json"; }                    // Вхідний слой
                else{ fileName = path +  "hiddenLayer_perceptronsWeights_" + std::to_string(layer) + ".json";}    // Приховані слої
            }

            openFile(fileName);

            j = json::parse(in_file);

            for(int perceptron = 0; perceptron < layers[layer]->quantityOfPerceptrons; perceptron++)
            {
                for(int weight = 0; weight < layers[layer]->quantityOfWeights; weight++)
                {

                    j["Layers"][layer]["Perceptrons"][perceptron]["Weights"][weight] =
                    {{"weight", layers[layer]->perceptronsLayer[perceptron].weights[weight]}};

                }

            }

            for(int bias = 0; bias < layers[layer]->quantityOfPerceptrons; bias++)
            {
                j["Layers"][layer]["Biases"][bias] = {{"weight", layers[layer]->bias.weights[bias]}};

            }

            saveAndCloseFile(fileName);
            j.clear();

        }

        return 1;
}

bool loadWeights(std::string path, int numbOfLayer)
{
    std::string fileName;
    // Визначаю назву файла
    if(numbOfLayer == countLayers-1)
    { fileName = path +  "outputLayer_perceptronsWeights.json"; }         // Вихідний слой
    else
    {   if(numbOfLayer == 0){ fileName = path +  "inputLayer_perceptronsWeights.json"; }                    // Вхідний слой
        else{ fileName = path +  "hiddenLayer_perceptronsWeights_" + std::to_string(numbOfLayer) + ".json";} // Приховані слої
    }


    using json = nlohmann::json;

    std::ifstream file;
    json j;

    file.open(fileName);


    j = json::parse(file);

    file.close();

    for(int perceptron = 0; perceptron < layers[numbOfLayer]->quantityOfPerceptrons; perceptron++)
    {
        for(int weight = 0; weight < layers[numbOfLayer]->quantityOfWeights; weight++)
        {
            layers[numbOfLayer]->perceptronsLayer[perceptron].weights[weight] =
                    j["Layers"][numbOfLayer]["Perceptrons"][perceptron]["Weights"][weight]["weight"];
            //qDebug() << "L: " << numbOfLayer << " P: " << perceptron << " W: " << weight << " = " << layers[numbOfLayer]->getPerceptronsArray()[perceptron].weights[weight];
        }
        layers[numbOfLayer]->bias.weights[perceptron] = j["Layers"][numbOfLayer]["Biases"][perceptron]["weight"];
    }

    j.clear();

    return 1;
}




};

#endif // NEURAL_NETWORK

