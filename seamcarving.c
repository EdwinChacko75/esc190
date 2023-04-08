
#include "seamcarving.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "c_img.h"

int delta(struct rgb_img *im, int y, int x, int dy, int dx){
    if(dx != 0){
        if (x==0){
            int red = get_pixel(im, y, 1, 0) - get_pixel(im, y, im->width -1,  0);
          int green = get_pixel(im, y, 1, 1) - get_pixel(im, y, im->width -1,  1);
           int blue = get_pixel(im, y, 1, 2) - get_pixel(im, y, im->width -1,  2);
            return red*red + blue*blue + green*green;

        } else if (x == im->width -1){
            int red = get_pixel(im, y, 0, 0) - get_pixel(im, y, x - 1, 0);
          int green = get_pixel(im, y, 0, 1) - get_pixel(im, y, x - 1, 1);
           int blue = get_pixel(im, y, 0, 2) - get_pixel(im, y, x - 1, 2);
            return red*red + blue*blue + green*green;

        }
        else{
            int red = get_pixel(im, y, x+1, 0) - get_pixel(im, y, x-1, 0);
          int green = get_pixel(im, y, x+1, 1) - get_pixel(im, y, x-1, 1);
           int blue = get_pixel(im, y, x+1, 2) - get_pixel(im, y, x-1, 2);
            return red*red + blue*blue + green*green;

        }
    }
    else if (dy != 0){
        if (y==0){
            int red = get_pixel(im, im->height - 1, x, 0) - get_pixel(im, 1, x, 0);
          int green = get_pixel(im, im->height - 1, x, 1) - get_pixel(im, 1, x, 1);
           int blue = get_pixel(im, im->height - 1, x, 2) - get_pixel(im, 1, x, 2);
            return red*red + blue*blue + green*green;

        } else if (y == im->height -1){
            int red = get_pixel(im, 0, x, 0) - get_pixel(im, y-1, x, 0);
          int green = get_pixel(im, 0, x, 1) - get_pixel(im, y-1, x, 1);
           int blue = get_pixel(im, 0, x, 2) - get_pixel(im, y-1, x, 2);
            return red*red + blue*blue + green*green;
        }
        else{
            int red = get_pixel(im, y+1, x, 0) - get_pixel(im, y-1, x, 0);
          int green = get_pixel(im, y+1, x, 1) - get_pixel(im, y-1, x, 1);
           int blue = get_pixel(im, y+1, x, 2) - get_pixel(im, y-1, x, 2);
            return red*red + blue*blue + green*green;
        }
    }
}
void calc_energy(struct rgb_img *im, struct rgb_img **grad){
    create_img(grad, im->height,im->width);
    int y = 0; int x = 0;
    for(y; y<im->height;y++){
        x=0;
        for(x; x < (im->width); x++){
            double energy = sqrt(delta(im,y,x,0,1) + delta(im,y,x,1,0))/10;
            int nrg = (int)energy;
            set_pixel(*grad,y,x, nrg,nrg,nrg);
        }

    }
}
void dynamic_seam(struct rgb_img *grad, double **best_arr){
    *best_arr = (double*)malloc(sizeof(double) * grad->height*grad->width);
    
    int i = 0;int j;
    int width = grad->width;
    int height = grad->height; // dosent matter

    for(i; i < height; i++){
        for(j=0; j < width;j++){
            if(i == 0 ){
                *(*best_arr+j) = get_pixel(grad, i, j, 0);
            }else if (i!=0){
                double min = -1.0; 

                for(int k = -1; k<2;k++){
                    double test;
                    if (j == 0 && k==-1 || j == width-1 && k==1){
                        continue;
                    }
                    else{
                        test = (*best_arr)[(i - 1) * width + j+k];
                    }
                    if(min>test || min == -1.0){
                        min = test;
                    }
                }
                (*best_arr)[i*width+j] = min + (double)get_pixel(grad, i, j, 0);
            }
        }
    }
}
void recover_path(double *best, int height, int width, int **path){
    *path = (int*)malloc(sizeof(int) * height);
    
    for(int i = height - 1; i>=0;i--){
        int min = -1;
        int min_i;
        for(int j = 0; j < width; j++){
            //printf("y: %d x: %d\n",i,j);
            int test = best[j +i*(width)];
            if(min > test || min == -1){
                min = test;
                min_i = j;
            }
        }
        (*path)[i] = min_i;
    }
}

void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path){
    int width = src->width-1;
    *dest = (rgb_img*)malloc(sizeof(rgb_img));
    (*dest)->width = width;
    (*dest)->height = src->height;
    (*dest)->raster = (uint8_t*)malloc(sizeof(int) * (src->height) * (width));
    for(int i = 0; i<src->height;i++){
        for(int j = 0; j< src->width;j++){
            if(j!=path[i]){
                (*dest)->raster[j+ i*width] = src->raster[j+i*src->width];
            }
        }
    }
}
