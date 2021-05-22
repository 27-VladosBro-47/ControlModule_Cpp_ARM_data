// #ifndef ALTAZIMUTH
// #define ALTAZIMUTH

// #include <wiringPi.h>
// #include <softPwm.h>

// #include "mediapipe/framework/port/opencv_highgui_inc.h"
// #include "mediapipe/framework/port/opencv_imgproc_inc.h"
// #include "mediapipe/framework/port/opencv_imgcodecs_inc.h"



// class AltAzimuth
// {
//     private:
//     //=======Змінні=======
    
//     // Якщо isWork == true, отже алгоритм відстеження працює
//     // інакше - припинити роботу альт-азимутальної установки
//     bool isWork;

//     // Вказівник на координати центра мас кистрі руки
//     // Якщо змінні рівні -1, отже кисть не виявлена і не має центра мас
//     cv::Point point;
//     // Координати центра зображення
//     cv::Point centre;
//     // Ширина і висота кадру зображення
//     int width, height;
//     // Відстанні, які будуть викоритовуватися при заданні зони
//     // у якій, при знаходжені point, сервоприводи не будуть рухатися
//     int calibrationW, calibrationH;
//     // d_x = centre.x - point->x
//     // d_y = centre.y - point->y
//     int d_x, d_y;

//     // Позиція сервоприводів
//     int positionX; // Вертикальне наведення (pin1=24)
//     int positionY; // Горизонтальне наведення (pin0=23)

//     // Номера пінів на Raspberry Pi
//     // pin0=23 - горизонтальне наведення [6, 24]
//     // pin1=24 - вертикальне наведення   [6, 26]
//     int pin0, pin1;
//     // Діапазон
//     int range;
//     // Значення
//     int value;

//     //====================

//     void tracking(int delayQuantity = 50)
//     {
//         d_x = centre.x - point.x;
//         d_y = centre.y - point.y;

//         setPositionX(delayQuantity);
//         setPositionY(delayQuantity);

//     }

//     void setPositionX(int delayQuantity)
//     {
//         if(d_x >= 0)
//         {
//             if(point.x < (centre.x - calibrationW) || point.x > (centre.x + calibrationW))
//             {
//                 if(positionX > 6)
//                 {softPwmWrite(pin1, --positionX); delay(delayQuantity);}
//                 else
//                 {/*setSignalTurn("left"); delay(delayQuantity);*/ std::cout << "TURN LEFT" << std::endl; delay(delayQuantity);}
//             }
//         }
//         else
//         {
//             if(point.x < (centre.x - calibrationW) || point.x > (centre.x + calibrationW))
//             {
//                 if(positionX < 26)
//                 {softPwmWrite(pin1, ++positionX); delay(delayQuantity);}
//                 else
//                 {/*setSignalTurn("right"); delay(delayQuantity);*/ std::cout << "TURN RIGHT" << std::endl; delay(delayQuantity);}
//             }
//         }
//     }

//     void setPositionY(int delayQuantity)
//     {
//         if(d_y >= 0)
//         {
//             if(point.y < (centre.y - calibrationH) || point.y > (centre.y + calibrationH))
//             {
//                 if(positionY <= 24)
//                 {softPwmWrite(pin0, ++positionY); delay(delayQuantity);}
//             }
//         }
//         else
//         {
//             if(point.y < (centre.y - calibrationH) || point.y > (centre.y + calibrationH))
//             {
//                 if(positionY > 6)
//                 {softPwmWrite(pin0, --positionY); delay(delayQuantity);}
//                 else
//                 {/*setSignalBack(positionX); delay(delayQuantity);*/ std::cout << "BACK: " << positionX << std::endl; delay(delayQuantity);}
//             }
//         }
//     }

//     // Функція, яка здійснює рух сервоприводів,
//     // що забезпечує сканування необхідної області
//     // delayQuantity - задає час затримки для сервоприводів
//     void searching(int delayQuantity = 150)
//     {
//         bool directionX = 0;
//         bool directionY = 0;

//         auto moveX = [&](){
//             if(directionX== 0) 
//             {softPwmWrite(pin1, --positionX); delay(delayQuantity);}
//             else 
//             {softPwmWrite(pin1, ++positionX); delay(delayQuantity);}
//         };
//         auto moveY = [&](){
//             if(directionY == 0) 
//             {positionY -= 2;softPwmWrite(pin0, positionY);delay(delayQuantity);}
//             else 
//             {positionY += 2;softPwmWrite(pin0, positionY); delay(delayQuantity);}
//         };

//         delay(500);

//         // Горизонталь
//         while(isWork==1 && point.x == -1)
//         {
//             // Якщо позиція сервопривода виходить із-за заданого діапазону
//             // то змінюємо напрямок руху
//             if(positionY <= 6 || positionY >= 24)
//             {
//                 directionY = !directionY; 
//             }

//             moveY();

//             // Вертикаль
//             while( (positionX > 6 && positionX < 26) && (isWork==1 && point.x == -1))
//             {
//                 moveX();          
//             }
//             directionX = !directionX;

//             if(directionX) {positionX = 7;}
//             else {positionX = 25;}
//         }

//     }

// public:

//     AltAzimuth(bool &isWork, cv::Point &point, int width = 640, int height = 480, pin0 = 23, pin1 = 24, range = 100, value = 50)
//     {
//         //---------------------------//

//         this->isWork = isWork;

//         this->point = point;

//         this->width = width;
//         this->height = height;

//         centre.x = width/2;
//         centre.y = height/2;

//         calibrationW = centre.x * 0.15;
//         calibrationH = centre.y * 0.15;

//         //---------------------------//

//         this->pin0 = pin0;
//         this->pin1 = pin1;

//         this->range = range;

//         this->value = value;

//         positionX = 17;
//         positionY = 15;

//         //---------------------------//
//         wiringPiSetup();

//         softPwmStop(pin0);
//         softPwmStop(pin1);
//         delay(10);
//         softPwmCreate(pin0, value, range);
//         softPwmCreate(pin1, value, range);
//         delay(10);
        
//         //---------------------------//

//         //Виставляємо сервоприводи у початкове положення
//         softPwmWrite(pin0, positionY);
//         softPwmWrite(pin1, positionX);

//         //---------------------------//
        
//     }
    
//     void mainLoop()
//     {
//         while(isWork)
//         {
//             if(point.x != -1)
//             {
//                 tracking();
//             }
//             else
//             {
//                 searching();
//             }

//         }
//     }

//     virtual ~AltAzimuth()
//     {

//     }


// };

// #endif // ALTAZIMUTH