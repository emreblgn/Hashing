#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define M 1009					// Hash tablosunun boyutunu tutan sabit degisken
#define STRSIZE 40				// Beklenen maksimum kelime uzunlugunu tutan sabit degisken

struct hash{					// Hash tablosunu tutan struct yapisi icerigi: kelime, ilk dokumanin struct yapisinin adresi, kelimenin kac dokumanda gectigini tutan sayi degeri.
	char word[STRSIZE];
	struct document *head;
	int docCount;
}; 

struct document{				// Kelimelerin bulundugu dokulanlarin isimlerini linked list yapisinda tutan struct yapisi.
	char name[STRSIZE];
	struct document *next;
};

struct hash *hashTable;			// Hash struct yapilarinin bulundugu dizinin pointeri.
int N = 0;						// Hash tablosunda bulunan eleman sayisi.
float lf;						// Load factor degeri.

int insertIntoHash(char*, char*);		// Girilen iki argumandan 1. si ile ayni isimde hash olusturup 2. si adinda dokuman adini bu hash yapisinin document linkli listesine ekleyen fonksiyon.
void writeFile();						// Hash tablosunun son halini "hash.txt" isimli dosyaya giren fonksiyon.
void readFile();						// "hash.txt" isimli dosyayi okuyup kaydedilen hash tablosunu hafizaya alan fonksiyon.
void readWordFile();					// Girilen txt dosyasindaki kelimeleri okuyup hash tablosuna kaydeden fonksiyon.
void loop();							// Kullanicidan farkli istekler almak uzere bu istekler bittiginde tekrar istek bekleyen dongu.
int writeLoadFactor();					// Load factor degerinin guncel halini ekrana yazdirip 0.8 degerinin uzerindeyse uyari veren fonksiyon.
int horner(char*);						// Arguman olarak gonderilen kelimenin horner metodu ile sayi degerini alan fonksiyon.
int docNameAvailable(int,char*);  		// Girilen 1. arguman adresindeki linkli listede  2. argumandaki isminde bir dosya olup olmadigi kontrol edilir.
void freeMemory();						// Bellekteki hash tablosunu ve dosya isimlerini temizleyen fonksiyon.

int main(){
	
	int i;
	hashTable = (struct hash*) calloc(M, sizeof(struct hash));			// Hash tablosu icin M degerine gore bellekte yer ayrilir.
	
	if(hashTable == NULL){												// Yer ayirmanin basarili olup olmadigi kontrol edilir.
		printf("Bellek ayrilamadi.");
		return 1;
	}
	
	FILE *fp;															// Programin bulundugu dizinde "hash.txt" dosyasi bulunup bulunmadigini kontrol edip buna gore kullaniciya bilgi veren sorgu.
	if((fp = fopen("hash.txt", "r")) == NULL){
		printf("Hash dosyasi bulunamadi program kapatilirken hash dosyasi olusturmak icin 4 nolu secenek ile cikis yapin.\n\n");
	}else{
		printf("Hash dosyasi bulundu.\n\n");
		readFile();
	}
	
	loop();																// Kullanici girislerinin alinacagi dongu.
	
	return 0;	
}

int insertIntoHash(char *word, char* docName){
	
	int i = 0;															// Dongu sayisini tutan degisken.
	int key = horner(word);												// Kelimenin sayi degeri alinmak icin horner metodu kullanilmak uzere kelime horner fonksiyonuna gonderilir.
	int sameWordFound = 0;												// Kelime zaten daha once girildiyse 1 yapilacak degisken.
	int emptyAddressFound = 0;											// Kelime icin bos bir adres bulunduysa 1 yapilacak degisken.
	int address;														// Kelimenin girilecegi adres degeri.
	
	while(sameWordFound == 0 && emptyAddressFound == 0){
		
		if(i>=M){														// Dongu sayisi M degerine ulasirsa tabloda bos yer kalmamis olacagi icin ekleme yapilamaz.
			return -1;
		}
		
		address = h(key, i);											// Adres degeri double hashing yontemi kullanilmak uzere key degeri ve i degeri h fonksiyonuna gonderilir.
		
		if(strlen(hashTable[address].word) == 0){						// Bulunan adreste kelime yoksa emptyAddressFound degiskeni 1 yapilir.
			emptyAddressFound = 1;
		}else if(strcmpi(hashTable[address].word, word) == 0){			// Adres bos degilse adresteki kelimeyle gecerli kelimenin ayni olup olmadigi kontrol edilir.
			sameWordFound = 1;
		}else{															// Adres farkli bir kelime ile doluysa double hashing ile yeni bir adres degeri olusturmak icin i degeri 1 arttirilir.
			i++;
		}
	}
	
	if(emptyAddressFound == 1){											// Bos adres bulunduysa kelime bu adrese yazilir, yeni bir dokuman yapisi tanimlanip gecerli kelimenin hash tablosundaki head degeri bu dosya ayarlanir.

		strcpy(hashTable[address].word, word);
		struct document *tmpDoc= (struct document *) malloc(sizeof(struct document));
		strcpy(tmpDoc->name, docName);
		tmpDoc->next = NULL;
		hashTable[address].head = tmpDoc;
		hashTable[address].docCount++;
		if(writeLoadFactor() == 1){										// Load factor kontrol edilip ekrana yazdirilir load factor 1 ise dosyadaki diger kelimelere bakmamak icin -1 degeri dondurulur.
			return -1;
		}
	}else if(docNameAvailable(address, docName) == 0){					// Adres bos degil ve ayni kelime ile doluysa gecerli dosya isminin bu adresteki dokuman linkli listesinde olup olmadigi kontrol edilir, dosya yoksa linkli listanin basina eklenir.
			struct document *tmpDoc= (struct document *) malloc(sizeof(struct document));
			strcpy(tmpDoc->name, docName);
			tmpDoc->next = hashTable[address].head;
			hashTable[address].head = tmpDoc;
			hashTable[address].docCount++;
	}
}

int docNameAvailable(int address,char* docName){						// Gonderilen 1. argumandaki adresteki dokuman linkli listesi tek tek gezilip 2. arguman ile ayni isimde bir dosya olup olmadigi kontrol edilir ayni isimde dosya varsa 1 degeri yoksa 0 degeri doner.
	
	struct document *tmpDoc = hashTable[address].head;
	
	while(tmpDoc != NULL){
		if(strcmp(tmpDoc->name, docName) == 0){
			return 1;
		}
		tmpDoc = tmpDoc->next;
	}
	return 0;
}

int horner(char *word){													// Girilen kelimeyi horner fonksiyonuna sokup hesaplanan sayi degerini donduren fonksiyon.
																		//NOT: r degeri 31 alindiginda long int degerini dahi asan sonuclar ciktigi ve negatif sonuclar donulup hata alindigi icin r degeri 3 olarak ayarlanmistir.
	int i;								
	int key = 0;
	int r = 3;
	
	for(i=0; i<strlen(word); i++){
		char c = word[i];
		if(c>96)														// Karakter kucuk harfse buyuk harf olarak ayarlanip isleme sokulur.
			c-=32;
		key += pow(r,(strlen(word) - i - 1)) * (c - 'A' + 1);
	}	
	return key;	
}

int h(int key, int i){													// Double hashing h fonksiyonu.
	return (h1(key) + i * h2(key)) % M;
}

int h1(int key){														// Double hashing h1 fonksiyonu.
	return key % M;
}

int h2(int key){														// Double hashing h2 fonksiyonu.
	return 1 + (key % (M - 1));
}

void writeFile(){														// Gecerli hash tablosunu "hash.txt" isimli dosyaya kaydeden fonksiyon.
					
	FILE *outfile;
	
	outfile = fopen("hash.txt", "w");									// Dosya, yazma formatinda acilir.
	if(outfile == NULL){
		fprintf(stderr,"Dosya acilamadi.");
		exit(1);
	}
	
	int i, j;
	
	for(i = 0; i<M; i++){												// Hash tablosundaki dolu ve bos butun veriler dosyaya yazdirilir.
		
		fwrite(&hashTable[i], sizeof(struct hash), 1, outfile);
		
		struct document *tmpDoc = hashTable[i].head;
		for(j = 0; j<hashTable[i].docCount; j++){						// Her kelimenin struct yapisindan hemen sonra bu kelimenin bulundugu dosyalarin isimleri ve isimlerinin kelime boyutlari pes pese yazdirilir.
			
			int nameLen = strlen(tmpDoc->name);
			fwrite(&nameLen, sizeof(int), 1, outfile);
			fwrite(tmpDoc->name, sizeof(char), nameLen, outfile);
			tmpDoc = tmpDoc->next;
		}
	}
	
	lf = (float) N / (float) M;											// Load factor degeri guncellenip son hali "hash.txt" dosyasinin sonuna eklenir.
	fwrite(&lf, sizeof(float), 1, outfile);
	printf("Yeni hash tablosu hash.txt isimli dosyaya kaydedildi.\n");
	fclose(outfile);													// Kaydedilen dosya kapatilir.
}

void readFile(){														// Kayitli hash tablosunu "hash.txt" isimli dosyadan alan fonksiyon.
	
	FILE *infile;
	infile = fopen("hash.txt","r");										// Dosya, okuma formatinda acilir.
	if(infile == NULL){
		fprintf(stderr,"Dosya acilamadi.");
		exit(1);
	}
	
	int i, j;
	for(i = 0; i<M; i++){												// Tablodaki butun dolu ve bos hash degerleri bellege alinir.
		
		fread(&hashTable[i], sizeof(struct hash), 1, infile);
		struct document tmpDoc;

		if(strlen(hashTable[i].word) != 0){								// Toplam kelime sayisi her alinan kelimede guncellenir.
			N++;
		}

		for(j = 0; j<hashTable[i].docCount; j++){						// Her kelimeden sonra bu kelimenin bulundugu dosya isimleri pes pese bulundugundan bunlarda dosyadan alinip linkli listeye eklenir.
			
			int nameLen;
			fread(&nameLen, sizeof(int), 1, infile);
			char *name = (char*) malloc(nameLen*sizeof(char));
			fread(name, sizeof(char), nameLen, infile);
			name[nameLen] = '\0';
			struct document *tmpDoc= (struct document *) malloc(sizeof(struct document));

			if(j==0){													// Alinan kelime ilk dosya ismiyse eklenince next degeri NULL yapilir.
				strcpy(tmpDoc->name, name);
				tmpDoc->next = NULL;
				hashTable[i].head = tmpDoc;
			}else{
				strcpy(tmpDoc->name, name);								// Alinan kelime ilk dosya degilse next degeri head deger yapilir.
				tmpDoc->next = hashTable[i].head;
				hashTable[i].head = tmpDoc;
			}
		}
	}
	
	fread(&lf, sizeof(float), 1, infile);								// Son olarak load factor degeri dosyadan cekilir.
	printf("Kayitli hash dosyasindaki load factor degeri = %f\n\n", lf);
	fclose(infile);														// Dosya kapatilir.
	
}

void readWordFile(){													// Kelime dosyasini okuyup hash tablosuna ekleyen fonksiyon.
	
	char tmpC;
	char newWord[STRSIZE];
	char fileName[STRSIZE];
	FILE *wordFile;
	int i, j = 0;
	
	printf("Kelimelerin bulundugu dosya adini girin: ");				// Dosya ismi kullanicidan alinir.
	scanf("%s", fileName);
	
	wordFile = fopen(fileName,"r");										// Dosya okuma formatinda acilir.
	if(wordFile == NULL){
		fprintf(stderr,"Dosya acilamadi.");
		exit(1);
	}
	
	fseek(wordFile, 0, SEEK_END);										// Dosyanin toplam boyutunu ogrenmek icin dosyanin pointeri dosyanin sonuna gonderilip degeri alinir ve pointer tekrar dosyanin basina gonderilir.
	int fileSize = ftell(wordFile);
	fseek(wordFile, 0, SEEK_SET);
	
	int hashFull = 0;													// Hash tablosunun dolu olup olmadigini tutan degisken.
	
	for(i=0; i<fileSize+1; i++){										// Dosyadaki butun karakterler icin for dongusu doner.
		tmpC = fgetc(wordFile);
		
		if(tmpC == ' ' || feof(wordFile)){								// Bosluga denk gelindiyse kelime bitmis demektir.
			if(hashFull == 0){
				int control = insertIntoHash(newWord, fileName);		// Gecerli kelime hash tablosuna eklenmeye calisilir.
				if(control == -1){										// insertIntoHash fonksiyonu -1 degerini donduyse hash tablosu dolu olacagindan hashFull degiskeni 1 yapilir ve kullaniciya uyari verilir.
					hashFull = 1;
					printf("[UYARI] Hash tablosu dolu. \nHash tablosuna eklenemeyen kelimeler: \n");
					printf("-%s\n",newWord);
				}
				memset(newWord, 0, j);									// Gecici kelime degiskeninin karakterleri sifirlanir.
				j = 0;
			}else{
				printf("-%s\n",newWord);								// Hash tablosu tamamen dolduysa tabloya eklenemeyen kelimeler sirayla ekrana yazdirilir.
			}
			
		}else{															// Bosluga denk gelinmediyse kelime devam ediyordur, gecerli karakter gecici kelime degerinin sonuna eklenir.
			newWord[j] = tmpC;
			j++;
		}
	}
	
	fclose(wordFile);													// Dosya kapatilir, kullaniciya bilgi verilir.
	printf("%s dosyasinin okuma islemi tamamlandi\n\n", fileName);
	printf("---------------------------------------\n\n");
	loop();																// Kullanicidan yeni istekler almak uzere loop dongusune geri donulur.
}


void printHash(){														// Hash tablosunun gecerli halini ekrana yazdiran fonksiyon.
	
	int i;
	
	for(i=0; i<M; i++){													// Tablodaki bos olmayan adreslerdeki kelimeler ve bu kelimelerin bulundugu dosya isimleri ekrana yazdirilir.
		if(strlen(hashTable[i].word) != 0){
			printf("[%d] adresindeki kelime: %s, bulundugu dosya sayisi: %d dosyalar:\n",i, hashTable[i].word, hashTable[i].docCount);
			
			struct document *tmp = hashTable[i].head;
			
			while(tmp->next != NULL){
				printf("%s, ", tmp->name);
				tmp = tmp->next;
			}
			printf("%s\n", tmp->name);
		}
	}
	printf("\n");														// Hash tablosunun maksimum boyutu, gecerli kelime sayisi, eklenebilecek kelime sayisi ve load factor degeri kullaniciya gosterilir.
	printf("Hash tablosunun boyutu: %d\nDolu adres sayisi: %d\nBos adres sayisi: %d\nLoad factor degeri: %f\n\n", M, N, M-N, lf);
	printf("---------------------------------------\n\n");
	loop();																// Kullanicidan yeni istekler almak uzere loop dongusune geri donulur.
}

void searchWord(char* word){											// Arguman olarak gonderilen kelimeyi hash tablosunda arayan fonksiyon.
	
	int i = 0;
	int fin = 0;														// Elemanin bulunma, bulunamama durumuna gore farkli deger alan degisken.
	int key = horner(word);												// Aranan kelimenin sayi degeri horner metodu ile bulunur.
	
	while(i<M && fin == 0){
		
		int address = h(key, i);										// Aranan kelimenin key degeri h fonksiyonuna gonderilip kelimenin uygun adres degeri bulunur.
		
		if(strlen(hashTable[address].word) == 0){						// Bos bir adrese gelindiyse kelime hash tablosunda yoktur bu yuzden donguden cikmak icin fin degeri -1 yapilir.
			fin = -1;							
		}else if(strcmpi(hashTable[address].word, word) == 0){			// Dolu bir adrese gelindiyse bu adresteki kelimenin aranan kelime olup olmadigi kontrol edilir, aranan kelime bulunduysa fin degeri 1 yapilir.
			fin = 1;
			printf("%s kelimesi hash tablosunda bulundu. Bulundugu dosyalarin isimleri:\n",word);
			struct document *tmp = hashTable[address].head;
			
			while(tmp->next != NULL){
				printf("%s, ", tmp->name);
				tmp = tmp->next;
			}
			printf("%s\n\n",tmp->name);
		}else{
			i++;														// Kelime bulunamadiysa ve devam edilecekse double hashing fonksiyonuna yollanmak uzere i degeri 1 arttirilir.
		}
		
	}
	if(fin != 1){														// Fin degeri 1 harici bir deger ise kelime hash tablosunda bulunamamistir.
		printf("%s kelimesi hash tablosunda bulunamadi.\n\n", word);
	}
	printf("---------------------------------------\n\n");
	loop();
}

int writeLoadFactor(){													// Load factor degerini hesaplayan fonksiyon.
	
	N++;
	lf = (float)N / (float)M;
	printf("Load Factor: %f", lf);										// Gecerli durumdaki load factor degeri ekrana yazdirilir.
	
	if(lf>=0.8 && lf < 1){												// Load factor degeri 0.8'den yuksek ise kullanici ayrica uyarilir.
		printf("(Dikkat load factor 0.8 degerini asti)");
	}else if(lf >= 1){													// Load factor degeri 1 olduysa kullaniciya bu bilgi verilip diger fonksiyonlarda kullanilmak uzere fonksiyon 1 degerini doner.
		printf("[UYARI] Load Factor degeri = 1, ekleme islemi durduruldu.\n");
		return 1;
	}
	printf("\n");
	return 0;
}

void freeMemory(){														// Bellekteki hash tablosunu ve dosya isimlerini temizleyen fonksiyon.
	
	int i;
	for(i=0; i<M; i++){
		
		struct document *tmpDoc = hashTable[i].head;
		
		while(tmpDoc != NULL){
			tmpDoc = tmpDoc->next;
			free(tmpDoc);
		}
	}
	
	free(hashTable);
	printf("Hafiza temizlendi.\n");
}

void loop(){															// Kullanicidan istek alinan dongu.
	
	int f;
	char word[STRSIZE];
	printf("Yapilacak islemi seciniz:\n");
	printf("1. Yeni kelime dosyasi gir.\n");
	printf("2. Hash dosyasinda kelime ara.\n");
	printf("3. Hash tablosunu ekrana yazdir.\n");
	printf("4. Cikis yap ve guncel hash tablosunu kaydet.\n");
	scanf("%d", &f);
	printf("---------------------------------------\n\n");
	
	
	switch(f){
		case 1:
			readWordFile();
			break;
		case 2:
			printf("Aranacak kelimeyi girin: ");
			scanf("%s", word);
			searchWord(word);
			break;
		case 3: 
			printHash();
			break;
		case 4:
			writeFile();
			freeMemory();
			break;
		default:
			printf("Hatali girdi.");
			loop();
	}
}
