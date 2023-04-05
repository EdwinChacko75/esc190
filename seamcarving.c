
#include "seamcarving.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "c_img.h"

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
