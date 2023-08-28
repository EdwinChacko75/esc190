#include "autocomplete.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
int compare(const void * a, const void * b) 
{
   return(strcmp(((term*)a)->term, ((term*)b)->term));
}
int weight_compare(const void * a, const void * b) 
{
   return (((term*)b)->weight-((term*)a)->weight);
}
void read_in_terms(term **terms, int *pnterms, char *filename){
    FILE *fp = fopen(filename,"r");
    char line[200];
    fgets(line,sizeof(line),fp);
    line[strlen(line) - 1] = '\0';
    *pnterms = atoi(line);
    *terms = (term *) malloc(*pnterms * sizeof(term));
    for(int i = 0; i <*pnterms; i++){
        fgets(line,sizeof(line),fp);
        line[strlen(line) - 1] = '\0';
        double weight;
        char * name = malloc(strlen(line) + 1);
        sscanf(line, "%le %[^\n]", &weight, name);
        (*terms)[i].weight = weight;
        strcpy((*terms)[i].term, name);
    }
    fclose(fp);
    qsort(*terms, *pnterms, sizeof(term), compare);
}
int lowest_match(term *terms, int nterms, char *substr){
    int start = 0;
    int end = nterms;
    int sublen = strlen(substr);
    int i = 0;
    while(start < end) 
    {
        int mid = floor((start + end) / 2);
        if (strncmp(substr, terms[mid].term, sublen) != 0 && strcmp(terms[mid].term, substr) < 0)
        {
            start = mid + 1;
        } else if (strncmp(substr, terms[mid].term, sublen) != 0 && strcmp(terms[mid].term, substr) > 0){
            end = mid;
        }else if(mid > 0 && strncmp(substr, terms[mid].term, sublen) == 0) {
            end = mid;
        }else {
            return mid;
        }
    }
    if (start == end){
        return start;
    }
    return -1; 
}
int highest_match(term *terms, int nterms, char *substr){
    int mid = floor(nterms/2);
    int len = strlen(substr);
    if (nterms <= 2){   
        if (strncmp(terms[mid].term, substr, len) == 0){
            return mid;
        }else if (strncmp(terms[mid-1].term, substr, len) == 0){
            return mid - 1;
        }else{
            return -1;
        }
    }else if (strncmp(terms[mid].term, substr, len) == 0 && strncmp(terms[mid].term, substr, len) > 0){
        return (highest_match(terms, mid, substr));
    } 
    else if(strncmp(terms[mid].term, substr, len) == 0){
        struct term* newterm = (terms + mid + 1);
        return (mid + 1 + highest_match(newterm, mid, substr));
    } 
    else if (strcmp(terms[mid].term, substr) > 0){
        return (highest_match(terms, mid, substr));
    }
    else if (strcmp(terms[mid].term, substr) < 0){   
        struct term* newterm = (terms + mid + 1);
        return (mid + 1 + highest_match(newterm, mid, substr));
    }
    return mid;
}
void autocomplete(term **answer, int *n_answer, term *terms, int nterms, char *substr){
    int high = highest_match(terms, nterms, substr);
    int low = lowest_match(terms, nterms, substr);
    if (high == -1 || low == -1){
        *n_answer = 0;
        return;
    }
    *n_answer = high - low + 1;
    *answer = (term*)malloc(sizeof(term) * (*n_answer));
    int counter = 0;
    for (int i = low; i < low + *n_answer; i++){
        strcpy((*answer + counter)->term, terms[i].term);
        (*answer + counter)->weight = terms[i].weight;
        counter++;
    }
    qsort(*answer, *n_answer, sizeof(term), weight_compare);
}