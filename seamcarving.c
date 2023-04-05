#include "seamcarving.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "c_img.h"
#include "c_img.h"
#include <stdio.h>
#include <math.h>

void create_img(struct rgb_img **im, size_t height, size_t width){
    *im = (struct rgb_img *)malloc(sizeof(struct rgb_img));
    (*im)->height = height;
    (*im)->width = width;
    (*im)->raster = (uint8_t *)malloc(3 * height * width);
}


int read_2bytes(FILE *fp){
    uint8_t bytes[2];
    fread(bytes, sizeof(uint8_t), 1, fp);
    fread(bytes+1, sizeof(uint8_t), 1, fp);
    return (  ((int)bytes[0]) << 8)  + (int)bytes[1];
}

void write_2bytes(FILE *fp, int num){
    uint8_t bytes[2];
    bytes[0] = (uint8_t)((num & 0XFFFF) >> 8);
    bytes[1] = (uint8_t)(num & 0XFF);
    fwrite(bytes, 1, 1, fp);
    fwrite(bytes+1, 1, 1, fp);
}

void read_in_img(struct rgb_img **im, char *filename){
    FILE *fp = fopen(filename, "rb");
    size_t height = read_2bytes(fp);
    size_t width = read_2bytes(fp);
    create_img(im, height, width);
    fread((*im)->raster, 1, 3*width*height, fp);
    fclose(fp);
}

void write_img(struct rgb_img *im, char *filename){
    FILE *fp = fopen(filename, "wb");
    write_2bytes(fp, im->height);
    write_2bytes(fp, im->width);
    fwrite(im->raster, 1, im->height * im->width * 3, fp);
    fclose(fp);
}

uint8_t get_pixel(struct rgb_img *im, int y, int x, int col){
    return im->raster[3 * (y*(im->width) + x) + col];
}

void set_pixel(struct rgb_img *im, int y, int x, int r, int g, int b){
    im->raster[3 * (y*(im->width) + x) + 0] = r;
    im->raster[3 * (y*(im->width) + x) + 1] = g;
    im->raster[3 * (y*(im->width) + x) + 2] = b;
}

void destroy_image(struct rgb_img *im)
{
    free(im->raster);
    free(im);
}


void print_grad(struct rgb_img *grad){
    int height = grad->height;
    int width = grad->width;
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            printf("%d\t", get_pixel(grad, i, j, 0));
        }
    printf("\n");    
    }
}

int delta(struct rgb_img *im, int y, int x, int dy, int dx){
    if(dx != 0){
        if (x==0){
            int red = get_pixel(im, 1, y, 0) - get_pixel(im, im->width -1, y, 0);
            int green = get_pixel(im, 1, y, 1) - get_pixel(im, im->width -1, y, 1);
            int blue = get_pixel(im, 1, y, 2) - get_pixel(im, im->width -1, y, 2);
            return red*red + blue*blue + green*green;

        } else if (x == im->width -1){
            int red = get_pixel(im, 0, y, 0) - get_pixel(im, im->width -2, y, 0);
            int green = get_pixel(im, 0, y, 1) - get_pixel(im, im->width -2, y, 1);
            int blue = get_pixel(im, 0, y, 2) - get_pixel(im, im->width -2, y, 2);
            return red*red + blue*blue + green*green;

        }
        else if (x != im->width -1 && x!=0){
            int red = get_pixel(im, x-1, y, 0) - get_pixel(im, x+1, y, 0);
            int green = get_pixel(im, x-1, y, 1) - get_pixel(im, x+1, y, 1);
            int blue = get_pixel(im, x-1, y, 2) - get_pixel(im, x+1, y, 2);
            return red*red + blue*blue + green*green;

        }
    }
    else if (dy != 0){
        if (y==0){
            int red = get_pixel(im, x, im->height - 1, 0) - get_pixel(im, x, 1, 0);
            int green = get_pixel(im, x, im->height - 1, 1) - get_pixel(im, x, 1, 1);
            int blue = get_pixel(im, x, im->height - 1, 2) - get_pixel(im, x, 1, 2);
            return red*red + blue*blue + green*green;

        } else if (y == im->height -1){
            int red = get_pixel(im, x, 0, 0) - get_pixel(im, x, im->height - 2, 0);
            int green = get_pixel(im, x, 0, 1) - get_pixel(im, x, im->height - 2, 1);
            int blue = get_pixel(im, x, 0, 2) - get_pixel(im, x, im->height - 2, 2);
            return red*red + blue*blue + green*green;
        }
        else{
            int red = get_pixel(im, x, y-1, 0) - get_pixel(im, x, y-1, 0);
            int green = get_pixel(im, x, y-1, 1) - get_pixel(im, x, y-1, 1);
            int blue = get_pixel(im, x, y-1, 2) - get_pixel(im, x, y-1, 2);
            return red*red + blue*blue + green*green;
        }
    }
}

void calc_energy(struct rgb_img *im, struct rgb_img **grad){
    create_img(grad, im->height,im->width);
    int y = 0; int x = 0;
    for(y; y<im->height;y++){
        x=0;
        for(x; x < im->width; x++){
            double energy = sqrt(delta(im,y,x,0,1) + delta(im,y,x,1,0))/10;
            int nrg = (int)energy;
            set_pixel(im,y,x, nrg,nrg,nrg);
        }

    }
}
void dynamic_seam(struct rgb_img *grad, double **best_arr){
    *best_arr = (double*)malloc(sizeof(double) * grad->height*grad->width);

    int i = 0;int j=0;
    int width = grad->width;
    int height = grad->height; // dosent matter
    for(i; i < width; i++){
        for(j; j < height;j++){
            if(i == 0 ){
                (*best_arr)[j] = grad[j];
            }else{
                double min = -1.0; 
                for(int k = -1; k<2;k++){
                    double test;
                    if (j == 0 && k==-1 || j == width-1 && k==1){
                        continue;
                    }
                    else{
                        test = grad[(i - 1) * width + j+k];
                    }
                    if(min>test || min = -1.0){
                        min = test;
                    }
                }
                (*best_arr)[i*width+j] = min;
            }
        }
    }
}

