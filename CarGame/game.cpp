#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include <iostream>
#include <queue>
#include <ctime>
#include <string.h>
#define wHeight 40 // height of the road
#define wWidth 100 // width of the road
#define lineX 45 // x coordinate of the middle line
#define lineLEN 10 // distance of the middle line from the beginning and the end
#define EXITY 35 // coordinate showing the end of the road
#define leftKeyArrow 260 // ASCII code of the left arrow key                 <
#define RightKeyArrow 261 // ASCII code of the right arrow key               <
#define leftKeyA 97// ASCII code of A                                        <
#define RightKeyD 100 // ASCII code of D                                     <
#define ESC 27 // // ASCII code of the ESC key                               <
#define ENTER 10 // ASCII code of the ENTER key                               <
#define KEYPUP 259 // ASCII code of the up arrow key                          <
#define KEYDOWN 258 // ASCII code of the down arrow key                       <
#define KEYERROR -1 // ASCII code returned if an incorrect key is pressed
#define SAVEKEY 115 // ASCII code of S                                        <
#define levelBound 300 // To increase level after 300 points
#define MAXSLEVEL 5 // maximum level
#define ISPEED 500000 // initial value for game moveSpeed
#define DRATESPEED 100000 // to decrease moveSpeed after each new level
#define MINX 5 // minimum x coordinate value when creating cars
#define MINY 10 // the maximum y coordinate value when creating the cars, then we multiply it by -1 and take its inverse
#define MINH 5 // minimum height when creating cars
#define MINW 5 // minimum width when creating cars
#define SPEEDOFCAR 3 // speed of the car driven by the player
#define YOFCAR 34 // y coordinate of the car used by the player
#define XOFCAR 45 // x coordinate of the car used by the player
#define IDSTART 10 // initial value for cars ID
#define IDMAX 20// maximum value for cars ID
#define COLOROFCAR 3 // color value of the car used by the player

#define POINTX 91 //x coordinate where the point is written
#define POINTY 42 //y coordinate where the point is written

#define MENUX 10 // x coordinate for the starting row of the menus             <
#define MENUY 5 // y coordinate for the starting row of the menus              <
#define MENUDIF 2 // difference between menu rows                              <
#define MENUDIFX 20 // difference between menu columns                         <

#define MENSLEEPRATE 200000 // sleep time for menu input
#define GAMESLEEPRATE 250000 // sleep time for player arrow keys
#define EnQueueSleep 1 // EnQueue sleep time
#define DeQueueSleepMin 2 // DeQueue minimum sleep time
#define numOfcolors 4 // maximum color value that can be selected for cars
#define maxCarNumber 5 // maximum number of cars in the queue
#define numOfChars 3 // maximum number of patterns that can be selected for cars


#define settingMenuItem 2 //  setting menu  secenek sayisi                     <
#define mainMenuItem 6 // ana menü seçenek sayisi                              <

using namespace std;

typedef struct Car{ //
    int ID;
    int x;
    int y;
    int height;
    int width;
    int speed;
    int clr;//arabanın rengidir ve rastgele belirlenir 1 ile 4 arasında
    bool isExist;
    char chr;
}Car;
typedef struct Game{
    int leftKey;
    int rightKey;
    queue<Car> cars;
    bool IsGameRunning;
    bool IsSaveCliked;
    int counter;
    pthread_mutex_t mutexFile;
    Car current;
    int level;
    int moveSpeed;
    int points;
}Game;

Game playingGame; // Global variable used for new game

const char *gameTxt =  "game.txt";
const char *CarsTxt =  "cars.txt";
const char *pointsTxt =  "points.txt";

//Array with options for the Setting menu
const char *settingMenu[50] = {"Play with < and > arrow keys","Play with A and D keys"};
const char *mainMenu[50] = {"New Game","Load the last game","Instructions", "Settings","Points","Exit"};
const char *instruction[100]={"< or A: moves the car to the left", "> or D: moves the car to the right", "ESC: exits the game without saving" ,"S: saves and exits the game"};


///--------------------Hocanın verdiği içeriği değiştirilmeyecek methodlar--------------------------------------
void initWindow(); //Creates a new window and sets I/O settings
void drawCar(Car c, int type, int direction);
void initGame(); // Assigns initial values to all control parameters for the new game
///-----------------------SEDA-----------------------------------------------------------
void printMenu(int selectedOption);//her seferinde kacýncý intexde ->iþaretinin olduðu bilgisini alarak menuyu ekrana yazdýrýr
void printSettings(int selectedOption);
void printPoints();
void MenuScreen();
void *newGame(void *); // manages new game
void printWindow(); //Draws the road on the screen
int  Random(int num);
void* moveCars(void *args);
void WritePoint(int point);
Car  madeCar();
///-----------------------ESMA-----------------------------------------------------------
void *dequeueCar(void *);
void *enqueueCar(void *);
void saveGame();
void saveCar(Car car);
void LoadLastGame();





int main()
{
    playingGame.leftKey = leftKeyArrow;
    playingGame.rightKey = RightKeyArrow;
   MenuScreen();
   return 0;

}

void initWindow()
{
	initscr();            // initialize the ncurses window
	start_color();        // enable color manipulation
	keypad(stdscr, true); // enable the keypad for the screen
	nodelay(stdscr, true);// set the getch() function to non-blocking mode
	curs_set(0);          // hide the cursor
	cbreak();             // disable line buffering
	noecho();             // don't echo characters entered by the user
	clear();              // clear the screen
    sleep(1);
}
///SEDA
void printMenu(int selectedOption) {
    int y=MENUY;
    char text[50];
    for (int i = 0; i<mainMenuItem; i++) {
        if (i == selectedOption) {
            strcpy(text,"->");
            strcat(text,mainMenu[i]);
            // Seçilen seçeneði kýrmýzý renkte ve ok iþareti ile yazdýrýr
            attron(COLOR_PAIR(2)); // Kýrmýzý renk etkinleþtirilir
            mvprintw(y,MENUX-2,text);// Menü seçeneði yazdýrýlýr
            attroff(COLOR_PAIR(2)); // Kýrmýzý rengi devre dýþý býrakýlýr
        } else{
             // Diðer seçenekleri normal renkte yazdýrýr
            attron(COLOR_PAIR(3)); // Beyaz renk etkinleþtirilir
            mvprintw(y,MENUX,mainMenu[i]) ;// Menü seçeneði yazdýrýlýr
            attroff(COLOR_PAIR(3)); // Beyaz rengi devre dýþý býrakýlýr
        }
        y=y+2;
        }
}
///SEDA
void printSettings(int selectedOption){
    int y=MENUY;
     char text[50];
    for (int i = 0; i<settingMenuItem; i++) {
        if (i == selectedOption) {
                strcpy(text,"->");
                strcat(text,settingMenu[i]);
            // Seçilen seçeneði kýrmýzý renkte ve ok iþareti ile yazdýrýr
            attron(COLOR_PAIR(2)); // Kýrmýzý renk etkinleþtirilir
            mvprintw(y,MENUX-2, text); // Menü seçeneði yazdýrýlýr
            attroff(COLOR_PAIR(2)); // Kýrmýzý rengi devre dýþý býrakýlýr
        } else {
            // Diðer seçenekleri normal renkte yazdýrýr
            attron(COLOR_PAIR(3)); // Beyaz renk etkinleþtirilir
            mvprintw(y,MENUX,settingMenu[i]); // Menü seçeneði yazdýrýlýr
            attroff(COLOR_PAIR(3)); // Beyaz rengi devre dýþý býrakýlýr
        }
        y=y+2;
    }

}
///SEDA
void printPoints() {

    int y = MENUY;
    int x=MENUX;
    char point[50];
    pthread_mutex_lock(&playingGame.mutexFile);
    FILE *file = fopen(pointsTxt, "r"); // sadece okumak için açacağım
    if (file == NULL) {
        printw("FILE NOT FOUND\n");
        refresh(); // Ekranı güncelle
    }else{
        int count=1;
        int blok=10;
        while (fgets(point, sizeof(point), file) != NULL) {
            if(count>blok){//demekki ilk blogu doldurduk yeni koordinat değerlerine ihtiyacımız var
                    x=x+MENUDIFX;//10 eklememizin sebbei son satırda game 10 : yazısı ile 10 satır ileri kaymıs olmamız
                    y=MENUY;
                    blok=blok+10;

            }

            if(count<=blok){///yani 10 dan kucukse/20/30/40 seklinde ilerliycek
                    attron(COLOR_PAIR(3));
                    mvprintw(y, x, " Game %d : %s",count, point);
                    attroff(COLOR_PAIR(3));
            }

            y += 2;
            count++;

        }

    }

     fclose(file);
     pthread_mutex_unlock(&playingGame.mutexFile);
     refresh();

}

///SEDA
void MenuScreen()
{
    initWindow();
    int selectedOption = 0;//Baslangıcta seçilen seçenek menu için
    int selectedForSetting=0; // baslangýcta secilen secenek settings kýsmý  için


    init_pair(1, COLOR_WHITE, COLOR_BLACK); // Renk çiftini tanýmlar (beyaz metin, siyah arkaplan)
    init_pair(2, COLOR_RED, COLOR_BLACK);   // Renk çiftini tanýmlar (kýrmýzý metin, siyah arkaplan)
    init_pair(3, COLOR_GREEN, COLOR_BLACK);//instructon kýsmýnýn yazý rengi

    printMenu(selectedOption); // Menüyü yazdýrýr
    bool IsExit=false;
    while (IsExit==false) {
        int ch = getch(); // Kullanýcýdan bir tuþ giriþi bekler
        switch(ch) {
            case KEYPUP:
                // Yukarý ok tuþu: Seçilen seçeneði bir yukarý taþýr
                if(selectedOption!=0){//ilk satýrda olmadýðý müddetçe selectedoption deðerini yukarý gitmek için azalt
                    selectedOption--;
                }
                break;
            case KEYDOWN:
                // Aşağı ok tuşu: Seçilen seçeneði bir asagı tasir
                if(selectedOption!=mainMenuItem-1){// son satırda olmadığı müddetçe selectedoption deðerini aþaðý inmek için arttýr
                    selectedOption++;
                }
                break;
            case ENTER:
            endwin();
            initWindow();
                /// ilk menuden entera basılma durumu**************************************

                if(selectedOption==0){///-------------------------------------------------------------newGame


                        initGame();
                        initWindow();
                        pthread_t th1; //create new thread
                        pthread_t thEnqueueCar, thDequeueCar;
                        pthread_create(&th1, NULL, newGame,NULL);// Run newGame function with thread
                        pthread_create(&thEnqueueCar,NULL,enqueueCar,NULL);
                        pthread_create(&thDequeueCar,NULL,dequeueCar,NULL);
                        pthread_join(th1, NULL);
                        pthread_join(thEnqueueCar,NULL);
                        pthread_join(thDequeueCar,NULL);

                }else if(selectedOption==1){///-------------------------------------------------------- Load the last game
                       LoadLastGame();
                       initWindow();
                        pthread_t th1; //create new thread
                        pthread_t thEnqueueCar, thDequeueCar;
                        pthread_create(&th1, NULL, newGame,NULL);// Run newGame function with thread
                        pthread_create(&thEnqueueCar,NULL,enqueueCar,NULL);
                        pthread_create(&thDequeueCar,NULL,dequeueCar,NULL);
                        pthread_join(th1, NULL);
                        pthread_join(thEnqueueCar,NULL);
                        pthread_join(thDequeueCar,NULL);


                }else if(selectedOption==2){    ///--------------------------------------------------- instructon

                    int y=MENUY ;
                    clear();
                    refresh();
                    for(int i=0;i<4;i++){
                        attron(COLOR_PAIR(3));
                        mvprintw(y,MENUX,instruction[i]);
                        attroff(COLOR_PAIR(3));
                        y=y+2;
                    }

                    refresh();
                    sleep(5);

                }else if(selectedOption==3){///-----------------------------------------------------------setting

                    clear();
                    refresh();
                    bool isExit=false;

                    while(isExit==false){
                        printSettings(selectedForSetting);
                        int chForSetting=getch();// Kullanýcýdan bir tuþ giriþi bekler
                        switch(chForSetting){
                            case KEYPUP:
                                if(selectedForSetting!=0){
                                    selectedForSetting--;
                                }
                                break;

                            case KEYDOWN:
                                if(selectedForSetting!=settingMenuItem){// son satýrda olmadýðý müddetçe selectedoption deðerini aþaðý inmek için arttýr
                                    selectedForSetting++;
                                }
                                break;
                            case ENTER:
                                if(selectedForSetting==0){

                                    playingGame.rightKey=RightKeyArrow;
                                    playingGame.leftKey=leftKeyArrow;

                                }else if(selectedForSetting==1){

                                    playingGame.leftKey=leftKeyA;
                                    playingGame.rightKey=RightKeyD;

                                }
                                isExit=true;

                        }
                        usleep(MENSLEEPRATE);
                        clear();
                        refresh();


                    }


                }else if(selectedOption==4){///--------------------------------------------------------------points
                    clear();
                    refresh();
                    printPoints();//point degerleri ekrana yazdırılır
                    sleep(5);
                    clear();
                    refresh();


                }else if(selectedOption==5){///---------------------------------------------------------------exit
                    IsExit=true;
                    break;

                }else{

                }
                endwin();
                initWindow();


        }

        clear(); // Ekraný temizler
        printMenu(selectedOption); // Menüyü güncellenmiþ olarak yazdýrýr
        usleep(MENSLEEPRATE);//uyuma zamaný
        // Ekraný günceller

    }
    endwin();  // ncurses modundan çýkýlýr


}
///-----------------------------------------------------------------------------------------------------------------


void initGame()
{
    playingGame.cars = queue<Car>();
    playingGame.counter =IDSTART;
    playingGame.mutexFile = PTHREAD_MUTEX_INITIALIZER; //assigns the initial value for the mutex
    playingGame.level = 1;
    playingGame.moveSpeed = ISPEED;
    playingGame.points = 0;
    playingGame.IsSaveCliked = false;
    playingGame.IsGameRunning = true;
    playingGame.current.ID = IDSTART-1; //current araba ID si 9
    playingGame.current.height = MINH;
    playingGame.current.width = MINW;
    playingGame.current.speed = SPEEDOFCAR;
    playingGame.current.x = XOFCAR;
    playingGame.current.y = YOFCAR;
    playingGame.current.clr = COLOROFCAR;
    playingGame.current.chr = '*';
}
///SEDA
void *newGame(void *)
{

    printWindow();
    drawCar(playingGame.current,2,1); // Draw the car the player is driving on the screen

    while (playingGame.IsGameRunning) { //continue until the game is over
          int key;
          key = getch(); //Get input for the player to press the arrow keys

            if (key != KEYERROR) {

                 if (key == playingGame.leftKey  && playingGame.current.x!=3) { // If the left  key is pressed
                        drawCar(playingGame.current,1,1); // removes player's car from screen
                        playingGame.current.x-=playingGame.current.speed; // update position
                        drawCar(playingGame.current,2,1); // draw player's car with new position
                }else if (key==playingGame.rightKey && playingGame.current.x!=(wWidth-10)){
                    drawCar(playingGame.current,1,1);//
                    playingGame.current.x+=playingGame.current.speed;
                    drawCar(playingGame.current,2,1);


                }else if(key==SAVEKEY){///burası loadLastGame methodu tamamlandıktan sonra yazılacak
                    saveGame();
                    playingGame.IsSaveCliked=true;
                     playingGame.IsGameRunning=false;

                    break;

                }else if(key==ESC){///
                    playingGame.IsGameRunning=false;
                    break;
                }
            }


         usleep(GAMESLEEPRATE); // sleep
        }
        return NULL;

}


///SEDA
void printWindow()
{
    init_pair(1, COLOR_WHITE, COLOR_BLACK); // Renk çiftini tanýmlar (beyaz metin, siyah arkaplan)
    init_pair(2, COLOR_RED, COLOR_BLACK);   // Renk çiftini tanýmlar (kýrmýzý metin, siyah arkaplan)
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    for (int i = 1; i < wHeight - 1; ++i) {
		//mvprintw: Used to print text on the window, paramters order: y , x , string
		attron(COLOR_PAIR(1));
        mvprintw(i, 2, "*"); //left side of the road
        mvprintw(i, 0, "*");
        mvprintw(i, wWidth - 1, "*");// right side of the road
        mvprintw(i, wWidth - 3, "*");
        attroff(COLOR_PAIR(1));

    }
    for (int i = lineLEN; i < wHeight -lineLEN ; ++i) { //line in the middle of the road
        attron(COLOR_PAIR(1));
        mvprintw(i, lineX, "#");
        attroff(COLOR_PAIR(1));

    }


    for(int i=5;i<35;i+=10){
        attron(COLOR_PAIR(3));
        mvprintw(i, wWidth+7 , "*");// right side of the road
        mvprintw(i+1, wWidth+6 , "*");
        mvprintw(i+1, wWidth+8 , "*");
        mvprintw(i+2, wWidth+5 , "*");
        mvprintw(i+2, wWidth+7 , "*");
        mvprintw(i+2,wWidth+9 , "*");
        attroff(COLOR_PAIR(3));

       attron(COLOR_PAIR(2));
       mvprintw(i+3, wWidth+7 , "#");
       mvprintw(i+4, wWidth+7 , "#");
       attroff(COLOR_PAIR(2));

    }

    refresh();
}



///ESMA
void *enqueueCar(void *)
{
    while(playingGame.IsGameRunning)
    {
        if(playingGame.cars.size()<=maxCarNumber)
        {
            Car newCar=madeCar();
            playingGame.cars.push(newCar);//qu ya araba verdik
            sleep(EnQueueSleep); //bekleme süresi
        }
    }
    return 0;
}
///ESMA
void *dequeueCar(void *)
{
 pthread_t thMoveCars;
    while(playingGame.IsGameRunning)
    { if(playingGame.cars.size()!=0){
         Car car=playingGame.cars.front(); //arabayı queue dan aldık
         playingGame.cars.pop();
         car.isExist=true;
         drawCar(car,2,0);
         pthread_create(&thMoveCars,NULL,moveCars,(void *)&car);
      }
      sleep(  (Random(3)+2 ) );
    }

    return 0;
}

///SEDA
int  Random(int num)
{

    srand(time(NULL));
    int number=rand()%num;
    return number;

}
///SEDA
void* moveCars(void *car)
{
    Car newCar = *(Car *)car; // Arabayı dışarıdan aldık
    while (newCar.isExist)
    {
        drawCar(newCar, 1, 0);
        newCar.y += (1 + rand() % newCar.speed); // Arabanın y koordinatını artırdık
        drawCar(newCar, 2, 0);

        // Çarpışma kontrolü
        if ((playingGame.current.y <= newCar.y + newCar.height) &&   // Mevcut araç diğer aracın altında veya çakışıyorsa
            (playingGame.current.y + playingGame.current.height >= newCar.y) &&  // Mevcut araç diğer aracın üstünde veya çakışıyorsa
            (playingGame.current.x <= newCar.x + newCar.width) &&  // Mevcut araç diğer aracın sağında veya çakışıyorsa
            (playingGame.current.x + playingGame.current.width >= newCar.x)) // Mevcut araç diğer aracın solunda veya çakışıyorsa
        {
            playingGame.IsGameRunning = false;
            WritePoint(playingGame.points);
            return NULL;

        }
        if(newCar.y>=35){
            newCar.isExist=false;
            drawCar(newCar, 1, 0);
            playingGame.points+=newCar.height*newCar.width;
            char point[10];
            sprintf(point, "%d", playingGame.points);
            mvprintw(POINTY,POINTX,"POINTS:");
            mvprintw(POINTY,POINTX+7,point);


            if(   (playingGame.level*300)<playingGame.points        &&  playingGame.level!=5  )
            {
                playingGame.level++;
                playingGame.moveSpeed+=100000;
            }
        }

        if(playingGame.IsSaveCliked){
            saveCar(newCar);
        }

        usleep(playingGame.moveSpeed); // Küçük bir bekleme süresi ekledik
    }

    return NULL;
}

///SEDA
void WritePoint(int point){
    pthread_mutex_lock(&playingGame.mutexFile);
    FILE *file=fopen(pointsTxt,"a+");
    fprintf(file,"%d\n",point);
    fclose(file);
    pthread_mutex_unlock(&playingGame.mutexFile);
}
///SEDA
Car madeCar()
{
    Car car;
    if(playingGame.counter==20){
    playingGame.counter=10;
    car.ID=playingGame.counter;

    }else{
        playingGame.counter++;
         car.ID=playingGame.counter;
    }

    car.y=Random(11)- 10;
    car.height=(Random(3)) + 5;//5 ile 7 arsında rastgele değer
    car.width=(Random(3)) + 5;
    car.speed=(car.height)/2;
    car.clr=(Random(4)) + 1;//1 ile 4 arasında rastgele deger alır
    car.isExist=true;
    int chrNum=(Random(3)) + 1;//1 ile 3 arsında rastgele deger alır
    switch(chrNum){
        case 1:
            car.chr='*';
            break;

        case 2:
            car.chr='#';
            break;

        case 3:
            car.chr='+';
            break;
    }

    //eger uretilen araba orta cızgıye değecek sekilde uretiliyorsa bastan x degeri uretilir
     bool inXLine=true;
     int xValue=0;
    while(inXLine)
    {
         xValue=Random(86)+5;
        for(int i=0;i<car.width;i++){
            if(  (i+xValue) == lineX){
                inXLine=true;
                break;

            }else{
                 inXLine=false;
            }
        }

    }
    car.x=xValue;

    return car;
}

///--------------------------------------------------------------------------------------------------------------------------
void drawCar(Car c, int type, int direction )
{
	//If the user does not want to exit the game and the game continues
    if(playingGame.IsSaveCliked!=true && playingGame.IsGameRunning==true)
    {
            init_pair(c.ID, c.clr, 0);// Creates a color pair: init_pair(short pair ID, short foregroundcolor, short backgroundcolor);
                                                            //0: Black (COLOR_BLACK)
                                                            //1: Red (COLOR_RED)
                                                            //2: Green (COLOR_GREEN)
                                                            //3: Yellow (COLOR_YELLOW)
                                                            //4: Blue (COLOR_BLUE)
			attron(COLOR_PAIR(c.ID));//enable color pair
            char drawnChar;
            if (type == 1 )
               drawnChar = ' '; // to remove car
            else
               drawnChar= c.chr; //  to draw char
		    //mvhline: used to draw a horizontal line in the window
			//shallow. : mvhline(int y, int x, chtype ch, int n)
			//y: horizontal coordinate
			//x: vertical coordinate
			//ch: character to use
			//n: Length of the line
            mvhline(c.y, c.x, drawnChar, c.width);// top line of rectangle
            mvhline(c.y + c.height - 1, c.x, drawnChar, c.width); //bottom line of rectangle
            if(direction == 0) // If it is any car on the road
                mvhline(c.y + c.height, c.x, drawnChar, c.width);
            else //player's card
                mvhline(c.y -1, c.x, drawnChar, c.width);
		    //mvvline: used to draw a vertical line in the window
			//shallow. : mvhline(int y, int x, chtype ch, int n)
			//y: horizontal coordinate
			//x: vertical coordinate
			//ch: character to use
			//n: Length of the line
            mvvline(c.y, c.x, drawnChar, c.height); //left line of rectangle
            mvvline(c.y, c.x + c.width - 1, drawnChar, c.height); //right line of rectangle
            char text[5];
            if (type == 1 )
                sprintf(text,"  "); //to remove point
            else
                 sprintf(text,"%d",c.height * c.width); // to show car's point in rectangle
            mvprintw(c.y+1, c.x +1, text);// display car's point in rectangle
            attroff(COLOR_PAIR(c.ID));// disable color pair
    }
}
///ESMA
void saveGame()
{   pthread_mutex_lock(&playingGame.mutexFile);
    FILE* gameFile=fopen(gameTxt,"wb");

    fwrite(&playingGame,sizeof(Game),1,gameFile);
    fclose(gameFile);
    pthread_mutex_unlock(&playingGame.mutexFile);
}
///ESMA
void saveCar(Car car)
{   pthread_mutex_lock(&playingGame.mutexFile);

    FILE* carFile=fopen(CarsTxt,"wb");
    fwrite(&car,sizeof(Car),1,carFile);
    fclose(carFile);
    pthread_mutex_unlock(&playingGame.mutexFile);
}
///ESMA
void LoadLastGame()
{
     FILE* gameFile=fopen(gameTxt,"rb");

    fread(&playingGame,sizeof(Game),1,gameFile);
    fclose(gameFile);
    playingGame.cars=queue<Car>();
    FILE* carsFile=fopen(CarsTxt,"rb");
    Car car;
    fread(&car,sizeof(Car),1,carsFile);

    while(!feof(carsFile))
    {
        playingGame.cars.push(car);
       fread(&car,sizeof(Car),1,carsFile);
    }
    fclose(carsFile);

}
