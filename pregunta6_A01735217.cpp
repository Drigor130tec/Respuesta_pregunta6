#include <iostream>
#include <string>
#include <omp.h>
#include <vector>
#include <cmath>
#define NUM_THREADS 500
using namespace std;



int matrixBlurring(unsigned char* image, long width, long height, int blurringLevel, int position){
   int mean = 0, row = 0, pivot = 0, pixel = 0;
   
   for (int i = -blurringLevel; i <= blurringLevel; i++){    //This loop iterates through the rows of the blur matrix
      pivot = position + i * width;
      if (pivot  >= 0 && pivot <= height*width){ //If the pixels of the pivot row are inside of the image
         row = (int)(pivot / width);
         for (int j = -blurringLevel; j <= blurringLevel; j++){ //This loop iterates through the columns of the blur matrix
            pixel = pivot + j;
            
            if (pixel >= (row * width) && pixel <= ((row + 1) * width - 1)){ //If the pixels are in the same row as the pivot
               mean += image[pixel];
            }
         }
      }
   }
   mean = mean / (pow((2 * blurringLevel + 1),2));
   //cout << mean << endl;
   return mean;
}

void blur(int num, int blurringLevel){

   FILE *inputImage, *outputImage, *lecturas;
   string in = "image" + to_string(num) + ".bmp";
   inputImage = fopen(in.c_str(),"rb");          //Imagen original a transformar
   string out = "img_" + to_string(num) + "blurred level_" + to_string(blurringLevel) + ".bmp";
   outputImage = fopen(out.c_str(),"wb");    //Imagen transformada
   long width;
   long height;
   unsigned char b, g, r;               

   unsigned char xx[54];
   for(int i=0; i<54; i++) {
   xx[i] = fgetc(inputImage);
   fputc(xx[i], outputImage);   //Copia cabecera a nueva imagen
   }
   width = (long)xx[20]*65536+(long)xx[19]*256+(long)xx[18];      //#R
   height = (long)xx[24]*65536+(long)xx[23]*256+(long)xx[22];


//-----------------------------------------------------------------------------------------
   unsigned char* arr_original = (unsigned char*)malloc(width*height*3*sizeof(unsigned char));
   unsigned char* arr_blurred  = (unsigned char*)malloc(width*height*3*sizeof(unsigned char));
   
   unsigned char* originalBlue = (unsigned char*)malloc(width*height*sizeof(unsigned char));
   unsigned char* originalGreen  = (unsigned char*)malloc(width*height*sizeof(unsigned char));
   unsigned char* originalRed = (unsigned char*)malloc(width*height*sizeof(unsigned char));
   unsigned char* blurredRed = (unsigned char*)malloc(width*height*sizeof(unsigned char));


   int k = 0;
   while(!feof(inputImage)){
         arr_original[k] = fgetc(inputImage);
         k++;
      }

   int j = 0;
   for(int i = 0; i < width*height*3; i+=3){
      originalBlue[j] = arr_original [i];
      originalGreen[j] = arr_original [i + 1];
      originalRed[j] = arr_original [i + 2];
      j++;
   }
   #pragma omp parallel
   {
   #pragma omp for
   for(int i = 0; i < width*height; i++){
      blurredRed[i] = matrixBlurring(originalRed, width, height, blurringLevel, i);
   }
   }
   //New Image
   j = 0;
   for(int i = 0; i < width*height*3; i += 3){
      arr_blurred[i] = originalBlue[j];
      arr_blurred[i + 1] = originalGreen[j];
      arr_blurred[i + 2] = blurredRed[j];
      j++;
   }

   for(int i = 0; i < width*height*3; i++){
      fputc(arr_blurred [i], outputImage);
   } 

   fclose(inputImage);
   fclose(outputImage);
   return;
}


int main(){
   int mSize[] = {4, 7, 11, 14, 17, 20};
   omp_set_num_threads(NUM_THREADS);
   //#pragma omp for

   double t1 = omp_get_wtime();
   for (int i = 0; i < 2; i++){
      blur(1,mSize[i]);
   }
   
   double t2=omp_get_wtime();
   double tiempo=t2-t1;
   printf("tomo (%lf) segundos\n", tiempo);
   return 0;
}