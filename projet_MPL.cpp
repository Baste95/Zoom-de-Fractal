//Projet Vittaut

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <complex>
#include <math.h>
#include <string>
#include <thread>
#include <vector>

using namespace cv; //Namespace d'opencv

//Complex Julia
constexpr std::complex<float> C(.285f, .013f);
int N_ITER = 300;  //Nombre d'itération (Plus il est élévé, plus de détails)


//Paramétre de la video
double FPS = 60;	//30 Frames par seconde
int NBR_SEC = 5;
int NBR_IMG = FPS * NBR_SEC;   //FPS * nbr de secondes
int WIDTH = 800;    //Hauteur de la video
int HEIGHT = 600;  //Largeur de la video

float ZOOM = 0.01f;

//Obligatoire pour le bon fonctionnement de la video
int cols, rows;


//Threads
std::vector<std::thread> myvector;
std::vector<cv::Mat> fractals;



//Fractal
Mat fractal(WIDTH, HEIGHT, CV_8UC3, Scalar(255,0,0));




/******** Fonction qui initilise le bon nombre d'itération pour un pixel ***********/
float julia_pix(float x, float y) {
    //std::lock_guard<std::mutex> lock(block);
    std::complex<float> z(x, y);
    int i = 0;
    while(i < N_ITER && abs(z) < 2){
        i++;
        z = z * z + C;
    }
    return (float)i*255/N_ITER;
}


/****** Fonction qui crée la fractale **********/
void createFractale( int debut_r, int fin_r){

    for(int r = debut_r; r < fin_r; ++r ){
        float y = -(float(r) - fractal.rows/ 2 ) * 0.5   * ZOOM;
        for(int c = 0; c < fractal.cols; ++c) {
            float x = (float(c) - fractal.cols / 2 ) * 0.5  * ZOOM;
            float ite = julia_pix(x, y) ;
            fractal.at<Vec3b>(r,c)[2] = ite + 2 - log(log(c * c + r * r)) / log(2);   //Red
            fractal.at<Vec3b>(r,c)[1] = ite + 2 - log(log(c * c + r * r)) / log(2);      //Green
            fractal.at<Vec3b>(r,c)[0] = ite + 2 - log(log(c * c + r * r)) / log(2);   //Blue
        }
    }
}



/********* Fonction qui crée le film  *****************/
void createMovie(int c, int r){
    Mat frame;
    Size outSize = Size(c, r);
    VideoWriter video("out.avi", CV_FOURCC('X', 'V', 'I', 'D'), FPS, outSize, true);
    for(int nbr = 1; nbr <= NBR_IMG; nbr++){
        std::ostringstream name;
        name << "./Images/img" << nbr << ".png";
        frame = imread(name.str());     // Insere chaque image précédement créées dans chaque frame
        video.write(frame);
    }
    video.release();
}

/******* Fonction qui initialise le nombre de threads que l'utilisateur veut ***********/
void launch_thread(int nbr_thread){
    int it_r = fractal.rows / nbr_thread;
    for(int i = 0, debut_r = 0; i < nbr_thread; i++){
        myvector.push_back(std::thread(createFractale, debut_r, debut_r + it_r ));
        debut_r += it_r;
    }
}


/******** Fonction Main ************/
int main(int argc, char** argv ){
    // int nbr = 1;
    int nbr_thread = 10;

    Size outSize = Size(WIDTH, HEIGHT);
    VideoWriter video("out.avi", CV_FOURCC('X', 'V', 'I', 'D'), FPS, outSize, true);

    while(NBR_SEC){

        launch_thread(nbr_thread);
        
        while(!myvector.empty()){
            myvector[0].join();
            myvector.erase(myvector.begin());
        }


        for(int w = WIDTH, h = HEIGHT; h < HEIGHT*2; w += WIDTH / 60, h += HEIGHT / 60){
            Mat fract;
            // std::cout << w << "====" << h << "\n";
            cv::resize(fractal, fract, cv::Size(w, h), CV_INTER_LINEAR);
            Mat f = Mat(fract,cv::Rect(w/2 - WIDTH/2, h/2 - HEIGHT/2, WIDTH, HEIGHT));
            imshow("Display Image", f);
            cvWaitKey(100);
            // std::ostringstream name;
            // name << "./Images/img" << nbr++ << ".png";
            // imwrite(name.str(), fract); //Enregistrement de l'image
            video.write(fract);
        }
        ZOOM /= 2.3f;
        N_ITER += 100;
        std::cout << N_ITER << "\n";
        NBR_SEC--;

    }
    video.release();
    // createMovie(fractal.cols, fractal.rows);

	return 0;
}
