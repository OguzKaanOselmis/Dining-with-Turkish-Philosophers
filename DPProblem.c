#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

enum philStates {ACIKMA, PILAV_YEME, AYRAN_ALMA, DUSUNME};
enum ayranStates {AYRAN_YOK,AYRAN_VAR}; 

enum philStates *state;         //filozoflarin stateleri  
enum ayranStates *ayranState;   //filozoflarin taslarinin stateleri

int *philosophers;
int philCount;          // Filozof sayisi
int surahiMusait;       // Surahinin musait olup olmadigini gosteren int 1 veya 0 degerlerini alir
int surahiKapasitesi;   // Tur basinda deger alacak K degeri 
int ilkSurahiKapasitesi;
int *philEatCount;  // Her filozofun ne kadar yedigini tutan array
int killCount;      // Bitmeyen son 1 kaldigini gosterecek olan degisken

pthread_t *thread_id;


sem_t mutex;        // Iki filozofun ayni anda catallari alma ve birakma islemlerini gerceklestirememesi icin
sem_t surahiMutex;   // Iki filozofun ayni anda surahi alma ve birakma islemlerini gerceklestirememesi icin
sem_t *self;        // i. Filozofun acikmis ancak catallara erisemedigi durumda beklemesi icin
sem_t *ayranSelf;   // i. Filozofun ayran almasi gerekirken ancak surahiye erisemedigi durumda beklemesi icin


void initialize(){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);    //thread'lere cancel atilabilmesini olanak saglar
    state = malloc(philCount*sizeof(enum philStates));
    ayranState = malloc(philCount*sizeof(enum ayranStates));
    philosophers = malloc(philCount*sizeof(int));
    self = malloc(philCount*sizeof(sem_t));
    ayranSelf = malloc(philCount*sizeof(sem_t));
    thread_id = malloc(philCount*sizeof(pthread_t));
    philEatCount = malloc(philCount*sizeof(int));

    surahiKapasitesi=rand()%(5*philCount)+1;    //Verilen filozof sayisinin 5 kati ile 1 arasinda rastgele bir deger verir
                                                //ve her ayran aliminda 1 azalir
    ilkSurahiKapasitesi = surahiKapasitesi;     //Son durumda yazilabilmesi icin
    
    surahiMusait = 1;
    killCount=0;

    for(int i=0;i<philCount;i++){
        philosophers[i] = i;
    }

    for(int i=0;i<philCount;i++){
        philEatCount[i] = 0;
    }

    for(int i=0;i<philCount;i++)
        state[i] = AYRAN_ALMA;      // Ayransiz masaya oturulmamasi icin inital AYRAN_ALMA state'i
    
    for(int i=0;i<philCount;i++)
        ayranState[i] = AYRAN_YOK;  // Initial ayran yok
        
    for(int i=0;i<philCount;i++){
        sem_init(&self[i], 0, 0);
        sem_init(&ayranSelf[i],0,0);
    } 

    sem_init(&mutex, 0, 1);
    sem_init(&surahiMutex, 0, 1);
}

void testForks(int philNum){    // Eger ayrani varsa, acikmissa ve yanindakiler yemiyor ise yemek yemege baslar

    if (state[philNum] == ACIKMA && ayranState[philNum] == AYRAN_VAR && state[(philNum+philCount-1) % philCount] != PILAV_YEME && state[(philNum+1) % philCount] != PILAV_YEME) {
        
        state[philNum] = PILAV_YEME;
 
        printf("Filozof %d, yemek yiyor\n", philNum + 1);
 
        sem_post(&self[philNum]);
    }
    
}

void pickUpForks(int philNum){  // testFork'u calistiran method ayni zamanda eger testi saglamiyor ise bekleyecegi yer
    sem_wait(&mutex);
    
    testForks(philNum);
    
    sem_post(&mutex);

    sem_wait(&self[philNum]);
}

void dropForks(int philNum){    // Testi saglayip yemegini bitirdigi durumda elindeki forklari birakmasini saglayan ve sag ve solu test eden method

    sem_wait(&mutex);

    printf("Filozof %d, catallari birakti, dusunme asamasina gecti\n", philNum + 1);

    philEatCount[philNum] += 1;

    state[philNum] = DUSUNME;

    testForks((philNum+philCount-1) % philCount);
    testForks((philNum+1) % philCount);

    sem_post(&mutex);

}

void testAyran(int philNum){    // Ayran doldurmak icin gerekli sartlar saglaniyor mu kontrolu

    if (state[philNum] == AYRAN_ALMA && ayranState[philNum] == AYRAN_YOK && surahiMusait) {
        surahiMusait=0;
        
        if(surahiKapasitesi>0)
            printf("\tFilozof %d, surahi ile ayran dolduruyor\n", philNum + 1);
        else{
            printf("\tFilozof %d, surahiyi aldi, ayran yok\n", philNum + 1);
        }
        
        sem_post(&ayranSelf[philNum]);
    }
}

void pourAyran(int philNum){    // testAyran'i cagiran method, test saglanmadigi durumda bekleyecegi yer
    sem_wait(&surahiMutex);
    
    ayranState[philNum] = AYRAN_YOK;
    state[philNum] = AYRAN_ALMA;

    testAyran(philNum);

    sem_post(&surahiMutex);

    sem_wait(&ayranSelf[philNum]);
}

void dropAyranDecanter(int philNum){    // Ayran doldurulduktan sonra diger ayran bekleyenlerin testini saglar
    sem_wait(&surahiMutex);

    ayranState[philNum] = AYRAN_VAR;
    state[philNum] = ACIKMA;

    printf("\tFilozof %d, surahiyi birakti\n", philNum + 1);
    surahiMusait=1;

    for(int i=0;i<philCount;i++){
        if(i!=philNum)
            testAyran(i);
    }

    surahiKapasitesi--;
    if(surahiKapasitesi<0){     // Eger surahi bitmis ise thread kendini oldurur ve main methoddaki join e girer
        state[philNum]=DUSUNME;
        killCount++;
        
        if(killCount==philCount-1){     // deadlock engelleme amacli bir onlem
            for(int i=0;i<philCount;i++){
                if(state[i]==0){
                    pthread_cancel(thread_id[i]);
                }
            }
        }
        sem_post(&surahiMutex);     // method devam etmeyeceginden surahiMutex'i salmasi gerekir
        pthread_exit(&thread_id[philNum]);
    }

    sem_post(&surahiMutex);
}

void* philosopher(void* num){
    while(1){
        int *philNum = num;

        pourAyran(*philNum);

        dropAyranDecanter(*philNum);
        
        pickUpForks(*philNum);

        dropForks(*philNum);
    }
}

int main(int argc, char* argv[])
{
    printf("Kac filozof olacagini giriniz:\n");
    scanf("%d",&philCount);
    int lapCount=1;
    if(philCount<1){
        printf("Yanlis input!\n");
        return 1;
    }
    
    while(1){
        printf("%d. TURUN BASLANGICI:\n",lapCount);
        initialize();
        
        for (int i = 0; i < philCount; i++) {
            pthread_create(&thread_id[i], NULL, philosopher, &philosophers[i]);
        }

        for (int i = 0; i < philCount; i++)
            pthread_join(thread_id[i], NULL);

        printf("----------------- %d. TURUN SONU -----------------\n",lapCount);
        printf("Surahideki total tas ayran sayisi %d\n", ilkSurahiKapasitesi);
        for(int i=0;i<philCount;i++){
            printf("Filozof %d yedigi yemek sayisi: %d\n",(i+1),philEatCount[i]);
        }
        printf("--------------------------------------------------\n");

        free(state);
        free(ayranState);
        free(philosophers);
        free(self);
        free(ayranSelf);
        free(thread_id);
        free(philEatCount);
        lapCount++;
        sleep(5);
    }

} 