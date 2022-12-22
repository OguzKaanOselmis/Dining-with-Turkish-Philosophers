# NewDiningPhilosophers
Multithreading, Pthreads, Process Synchronization konularını daha iyi kavrayabilmek için yazılmış bir kod

Yeni Özellik Eklenmiş Hali ile Dining Philosophers Problemi

Derlemek için yazılması gereken kod:

gcc -pthread -o dpp DPProblem.c

Çalıştırma Aşaması:

./dpp

Çalıştırıldıktan sonra filozof sayısının girilmesi yeterlidir.

Detaylar:
Normal Dining Philosophers problemi yeme-düşünme döngüsünden oluşmaktadır. Bu implementasyonda ekstra olarak Ayran alma aşaması bulunmaktadır. 
Süreç 3 aşamalıdır: Ayran Alma, Pilav Yeme ve Düşünme

Her filozof yemek masasına elinde 1 tas ayran olmadan geçememekte ve yemek sırasında tasındaki ayranın hepsini içmektedir. Yalnız, masada yalnızca bir tane ayran sürahisi vardır ve aynı anda yalnızca bir filozof bu sürahiden ayranı kendi tasına koyabilmektedir. Her turda sürahiye 1 ile 5*filozof_sayisi arasında bir değer kadar ayran konmaktadır ve sürahi bittiğinde tur bitmektedir. Tur sonunda hangi filozofun ne kadar yediği yazdırılmaktadır.
