#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

stbi_uc *load(char const *fileName){
    int width, height, channels;
    unsigned char *img = stbi_load(fileName, 
    &width, &height, &channels, 0);
    if (img == NULL){
        perror("failed to load image");
        exit(EXIT_FAILURE);
    }
    printf("loaded image with %d width, %d height and %d channels\n", width, height, channels);
    return img;
}

//Basics of how to load an image
int main(void){ 
    unsigned char *q1 = load("stb_image/Q1.jpg");
    unsigned char *q1Ans = load("stb_image/Q1Ans.jpg");
    unsigned char *q2 = load("stb_image/Q2.jpg");
    unsigned char *q2Ans = load("stb_image/Q2Ans.jpg");

    
    stbi_image_free(q1);
    stbi_image_free(q1Ans);
    stbi_image_free(q2);
    stbi_image_free(q2Ans);

/* 
    How to make an image grey scale
    size_t img_size = width * height * channels;
    int grey_channels = channels == 4 ? 2 : 1;
    size_t grey_img_size = width * height * grey_channels;

    unsigned char *grey_img = malloc(grey_img_size);
    if(grey_img == NULL) {
        printf("Unable to allocate memory for the gray image.\n");
        exit(EXIT_FAILURE); 
    }

    for(unsigned char *p = img, *pg = grey_img; p != img + img_size; 
    p += channels, pg += grey_channels) {
        *pg = (uint8_t)((*p + *(p + 1) + *(p + 2))/3.0);
        if(channels == 4) {
            *(pg + 1) = *(p + 3);
        }
    }

    stbi_write_jpg("stb_image/EveryItemImageGrey.jpg", width, height, grey_channels, grey_img, 100);
    free(grey_img);
*/

}