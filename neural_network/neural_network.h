#ifndef NEURAL_NETWORK
#define NEURAL_NETWORK

#include "perceptrons.h"
#include "mediapipe/examples/desktop/controle_module/json_lib/json.hpp"

#include <iomanip>
#include <fstream>
#include <ctime>
#include <cmath>
#include <string>
#include <vector>
#include <thread>
#include <filesystem>
#include <iostream>

class NeuralNetwork
{
private:
    // Зберігає кількість слоїв у нейромережі
    int countLayers;
    // Масив вказівників на слої нейромережі
    std::vector<Perceptrons*> layers;

    bool isLearning;

    // Тип жесту
    std::string typeOfData;
    // Номер жесту
    int numbOfMotion;
    // Вектор, що містить дані для вхідного шару нейромережі
    std::vector<double> data;
    
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


void study(std::string type)
{
    // Змінна міститиме кількість папок із начальними даними
    // і кількість навчальних даних у кожній з папок
    // Кількість елементів вектора - кількість папок
    // Значення окремого елемента - кількість файлів
    std::vector<int> dirsAndFiles;

    // Змінна містить загальну кількість навчальних даних з усіх папок
    int quantityOfFiles;


    if(type == "hand")
    {   
        std::cout << "Start neural network`s learning." << std::endl;

        // Змінюю ємність вектора відповідно до типу жесту
        // Дані про руку містять 63 змінні (21*3=63)
        data.resize(63);

        // Змінна зберігатиме порядковий номер файла із папки,
        // який необхідно буде наступним завантажити
        // Наприклад i[3] = 2 : у папці "3", третій файл із папки треба буде завантажити
        std::vector<int> currFiles = dirsAndFiles;
        // Зануляємо (виставляємо перший файл у папці)
        for(auto &i : currFiles) { i = 0; }

        // Отримую дані про кількість папок із навч даними
        // і кількість самих даних у папках
        calculateDirsAndFiles(type, dirsAndFiles, quantityOfFiles);

        bool flag = 1;
        int dir = 0;
        while(flag)
        {
            if(currFiles[dir] < dirsAndFiles[dir])
            {
                loadTrainData(type, dir, currFiles[dir], typeOfData, numbOfMotion, data);
                ++currFiles[dir];

                setInputPerceptrons(type, data);
                calculateNeuralNetwork();
                // numbOfMotion буде співпадати з dir, можна використати dir
                setTrueAnswer(numbOfMotion);
                backPropagation();

            }

            dir++;
            if(dir == dirsAndFiles.size()) { dir = 0;}

            flag = 0;
            for(int i = 0; i < dirsAndFiles.size(); i++)
            {
                if(currFiles[dir] != dirsAndFiles[dir])
                {
                    flag = 1;
                }
            }
        }

        // for(int numbFile = 0; numbFile < quantityOfFiles; numbFile++)
        // {
        //     QString path = scene->getImgName(dirNumb);

        //     scene->getRGBfromImgForLearning(path);

        //     Neural->setInputPerceptrons(scene->getPixelsArrayPtr(),28,28);
        //     Neural->calculateNeuralNetwork();

        //     Neural->setTrueAnswer(dirNumb);

        //     Neural->backPropagation();

        //         qDebug() << "Number = " << numb;
        //         qDebug() << "\niteration = " << 1+dirNumb << "\nlevel of Education = " << Neural->levelOfEducation();

        //         ui->label_49->setText(QString::number(numb+1));
        //         ui->label_50->setText(QString::number(dirNumb));
        //         ui->label_51->setText(QString::number(Neural->levelOfEducation()));
        //         ui->label_55->setText(QString::number(scene->getNumbOfImg()));
        //         emit sendNumb(numb);


        //     dirNumb++;
        //     if(dirNumb>9)
        //         dirNumb = 0;

        //     if(isLearning==0)
        //     {
        //         g_dirNumb = dirNumb;
        //         g_numbOfPicture = numb+1;
        //         break;
        //     }

        //     if(numb==(scene->getNumbOfImg() - 1))
        //     {
        //         emit sendNumb((scene->getNumbOfImg()));
        //         g_dirNumb = 0;
        //         g_numbOfPicture = 0;
        //     }
        // }

        std::cout << "Finish neural network`s learning." << std::endl;
    }

    
}


void calculateDirsAndFiles(std::string type, std::vector<int> &dirsAndFiles_out, int &quantityOfFiles_out)
{
    using json = nlohmann::json;
    namespace fs = std::filesystem;

    std:: string currPath;
    std:: string pathToFolder;
    std:: string pathToSubFolder;

    if(type == "hand")
    {
        currPath =  fs::current_path();
        pathToFolder = currPath + "/mediapipe/examples/desktop/controle_module/train_data/train_data_hands";

        // Підраховую кількість файлів у папці
        int countFiles = 0;
        // Кількість папок задасть кількість ітерацій для сканування
        int numbOfDirs = getQuantityOfItemInFolder(pathToFolder);
        // Змінюю розмір вектора відповідно до кількості файлів
        dirsAndFiles_out.resize(numbOfDirs);
        for(int folder = 0; folder < numbOfDirs; folder++)
        {
            pathToSubFolder = pathToFolder + "/" + std::to_string(folder);
            // Записую кількість файлів у конкретній папці
            dirsAndFiles_out[folder] = getQuantityOfItemInFolder(pathToSubFolder);
            countFiles += dirsAndFiles_out[folder];
        }

        quantityOfFiles_out = countFiles;
    }
}

// Підрахунок кількості елементів у папці
// (не рахуючи елементи інших папок, які містяться в даній папці)
// Наприклад pathToFolder== "/mainDir/dir0/folder"
// пошук буде здійснюватися у папці folder
int getQuantityOfItemInFolder(std::string pathToFolder)
{
      namespace fs = std::filesystem;

      // Підраховую кількість файлів у папці
      int count = 0;
      
      fs::path pathToFiles(pathToFolder);
      for (const auto & entry : std::filesystem::directory_iterator(pathToFiles))
      {
        //std::cout << entry.path() << std::endl;
        ++count;
      }
      
      return count;
}

void saveTrainData(std::string type , int numbOfFolder /*Номер жеста*/, const std::vector<double> &data)
{ 
    // Визначаю простір імен
    using json = nlohmann::json;
    namespace fs = std::filesystem;

    //==========Змінні===============
    int countFiles = 0;           // Змінна зберігає кількість виявлених файлів у теці

    std::ifstream in_file;        // Файлове введеня (Читання з файлу)
    std::ofstream out_file;       // Файлове виведення (Запис у файл)
    json j;                       // Змінна, 

    std::string currPath;         // Змінна містить шлях до поточеої робочої теки

    std::string fileName;         // Змінна містить повний шлях до необхідного файлуv

    std::string pathToFolder;     // Шлях до основної папки з навч даними (Наприклад папка train_data_hands)
    std::string pathToSubFolder;  // Шлях до підпапок (папки 0, 1, 2, 3, 4...)
    //===============================


    //========Лямбда-вирази==========
    // Лямбда-вираз, що забезпечує відкриття файлу
    // Або створює файл, якщо файлу не існує
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

    // Лямбда-вираз, що забезпечує зберігання файлу
    auto saveAndCloseFile = [&](std::string fileName)
    {
        in_file.close();
        out_file.open(fileName);

        out_file << std::setw(4) << j << std::endl;
        out_file.close();
    };
    //===============================


    // Якщо тип даних "hand" - виконуємо відповідну процедуру
    if(type == "hand")
    {
      currPath =  fs::current_path();
      pathToFolder = currPath + "/mediapipe/examples/desktop/controle_module/train_data/train_data_hands";

      // Підраховую кількість файлів у папці
      countFiles = 0;
      // Кількість папок задасть кількість ітерацій для сканування
      int numbOfDirs = getQuantityOfItemInFolder(pathToFolder);
      for(int folder = 0; folder < numbOfDirs; folder++)
      {
        pathToSubFolder = pathToFolder + "/" + std::to_string(folder);
        countFiles+=getQuantityOfItemInFolder(pathToSubFolder);
      }

      fileName = pathToFolder + "/" + std::to_string(numbOfFolder) + "/" + std::to_string(countFiles) + ".json";
      openFile(fileName);

      // Парсимо дані з файлу який відкрили (створили)
      j = json::parse(in_file);

      // Розпочинаємо запис даних у файл
      j["Type"] = "hand";                   // Тип жестикуляції
      j["Numb_of_motion"] = numbOfFolder;           // Номер жеста
      for(int i = 0; i < data.size(); i++)  // Запис навчальних даних
      {
        j["Input"][i] = data[i];
      }
      // Зберігання файлу
      saveAndCloseFile(fileName);
      j.clear();

    }

}

void loadTrainData(std::string type, int numbOfFolder, int numbOfFile, std::string &type_out, int &numb_out, std::vector<double> &data_out)
{
  // Визначаю простір імен
  using json = nlohmann::json;
  namespace fs = std::filesystem;

  //==========Змінні===============
    int countFiles = 0;         // Змінна зберігає кількість виявлених файлів у теці

    std::ifstream in_file;      // Файлове введеня (Читання з файлу)
    std::ofstream out_file;     // Файлове виведення (Запис у файл)
    json j;                     // Змінна, 

    std::string currPath;      // Змінна містить шлях до поточеої робочої теки

    std::string fileName;       // Змінна містить повний шлях до необхідного файлу
    std::string pathToFolder;   // Шлях до папки з даними
    //===============================

  if(type == "hand")
  {
    
    // Формування шляху до файлу
    currPath =  fs::current_path();
    fileName = currPath + "/mediapipe/examples/desktop/controle_module/train_data/train_data_hands/"
    + std::to_string(numbOfFolder) + "/" + std::to_string(numbOfFile) + ".json";

      currPath =  fs::current_path();
      pathToFolder = currPath + "/mediapipe/examples/desktop/controle_module/train_data/train_data_hands/" + std::to_string(numbOfFolder);


      int count = 0;
      fs::path pathToFiles(pathToFolder);
      for (const auto & entry : std::filesystem::directory_iterator(pathToFiles))
      {
        if(count == numbOfFile)
        { fileName = entry.path(); std::cout << fileName << std::endl; break;}
        ++count;        
      }

    // Відкриття файлу
    in_file.open(fileName);
    if(!in_file.is_open())
    {
      std::cerr << "loadTrainData : Can`t open the file:" << std::endl;
      std::cerr << fileName << std::endl;
    }

    // Парсинг файлу
    j = json::parse(in_file);

    // Закриття файлу
    in_file.close();

    type_out = j["Type"];
    numb_out = j["Numb_of_motion"];

    data_out.resize(63);
    for(int i = 0; i < data_out.size(); i++)  // Запис навчальних даних
    {
      data_out[i] = j["Input"][i];
    }

    j.clear();

  }
}


};

#endif // NEURAL_NETWORK

