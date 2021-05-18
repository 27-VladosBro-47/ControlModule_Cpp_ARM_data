//#include "mediapipe_hands_detector/mediapipe_hands_detector.h"
#include "hand_tracking/hand_tracking.h"

#include <filesystem>

// Перевірка аргументів командного рядка
// Повертає string який позначає режим, у якому має працювати програма
std::string setMode(int argc, char** argv)
{
  if(argc > 1)
  {
    std::string agrument = argv[1];

    if(agrument == "default" || agrument == "DEFAULT" || agrument == "Default")
    {
      return "default";
    }
    else if(agrument == "learning" || agrument == "LEARNING" || agrument == "Learning")
    {
      return "learning";
    }
    else if(agrument == "make_data" || agrument == "MAKE_DATA" || agrument == "Make_data")
    {
      return "make_data";
    }
    else
    {
      std::cout << "Incorrect command line argument." << std::endl;
      std::cout << "Recognized as \"default\"." << std::endl;
      return "default";
    }
  }
  else
  {
    std::cout << "Command line argument is empty." << std::endl;
    std::cout << "Recognized as \"default\"." << std::endl;
    return "default";
  }

}


void saveTrainData(std::string type , int numbOfFolders /*Номер жеста*/, const std::vector<double> &data)
{
    // Визначаю простір імен
    using json = nlohmann::json;
    namespace fs = std::filesystem;

    //==========Змінні===============
    int countFiles = 0;         // Змінна зберігає кількість виявлених файлів у теці

    std::ifstream in_file;      // Файлове введеня (Читання з файлу)
    std::ofstream out_file;     // Файлове виведення (Запис у файл)
    json j;                     // Змінна, 

    std::string curr_path;      // Змінна містить шлях до поточеої робочої теки

    std::string fileName;       // Змінна містить повний шлях до необхідного файлу
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
      curr_path =  fs::current_path();
      curr_path = curr_path + "/mediapipe/examples/desktop/controle_module/train_data_hands/" + std::to_string(numbOfFolders);

      // Підраховую кількість файлів у папці
      countFiles = 0;
      fs::path pathToFiles(curr_path);
      for (const auto & entry : std::filesystem::directory_iterator(pathToFiles))
      {
        //std::cout << entry.path() << std::endl;
        ++countFiles;
      }

      fileName = curr_path + "/" + std::to_string(countFiles) + ".json";
      openFile(fileName);

      // Парсимо дані з файлу який відкрили (створили)
      j = json::parse(in_file);

      // Розпочинаємо запис даних у файл
      j["Type"] = "hand";                   // Тип жестикуляції
      j["Numb_of_motion"] = numbOfFolders;           // Номер жеста
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

    std::string curr_path;      // Змінна містить шлях до поточеої робочої теки

    std::string fileName;       // Змінна містить повний шлях до необхідного файлу
    //===============================

  if(type == "hand")
  {
    // Формування шляху до файлу
    curr_path =  fs::current_path();
    fileName = curr_path + "/mediapipe/examples/desktop/controle_module/train_data_hands/"
    + std::to_string(numbOfFolder) + "/" + std::to_string(numbOfFile) + ".json";

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



int main(int argc, char** argv)
{
  namespace fs = std::filesystem;

  std::string mode;
  mode = setMode(argc, argv); 

//========================================//
// std::string buff =  fs::current_path();
// buff = buff + "/mediapipe/examples/desktop/controle_module/train_data_hands/0";

// fs::path pathToShow(buff);
// for (const auto & entry : std::filesystem::directory_iterator(pathToShow))
// {
//   std::cout << entry.path() << std::endl;
// }
//========================================//

//========================================//
// std::vector<double> data;
// data.resize(63);
// double extra = 0.1;
// for(auto &count : data)
// {
//   count = 1 + extra;
//   extra+=0.1;
// }

// saveTrainData("hand", 1, data);

// data.clear();

// std::string type_out;
// int numb_out;

// loadTrainData("hand", 1, 0, type_out, numb_out, data);

// std::cout << "==============================\n";
// std::cout << "type_out = " << type_out << std::endl;
// std::cout << "numb_out = " << numb_out << std::endl;
// for(int i = 0; i < data.size(); i++)
// {
//   std::cout << "i[" << i << "] = " << data[i] << std::endl;
// }
//std::cout << "==============================\n";
//========================================//


  if(mode == "default")
  {
    std::cout << "Start the program in \"" << mode << "\" mode." << std::endl;

    //HandTracking handTracking;
    //handTracking.processTracking();

  }
  else if(mode == "learning")
  {
    std::cout << "Start the program in \"" << mode << "\" mode." << std::endl;

  } 
  else if(mode == "make_data")
  {
    std::cout << "Start the program in \"" << mode << "\" mode." << std::endl;

  }





  return 0;
}