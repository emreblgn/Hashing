# Hashing
 Hashing Yöntemi İle Kelime Sözlüğü
 
**Program Özellikleri**

 1. Hash tablosuna yeni bir sözlük dosyasındaki kelimeleri ekleme.
 
 2. Hash tablosunda belirli bir kelimenin aranıp kelimenin bulunduğu sözlük dosyalarının isimlerinin ekrana yazdırılması.
 
 3. Hash tablosundaki bütün kelimelerin ve kelimelerin bulunduğu dosyaların isimlerinin ekrana yazdırılması.
 
 4. Hash tablosuna her eklenen kelimede güncel load factor değerinin ekrana yazdırılması ve load factor değeri 0.8'in üzerindeyse uyarı verilip 1 değerine geldiğinde ekleme işlemlerinin durdurulması.
 
 5. Program kapatılırken güncel hash tablosunun "hash.txt" isimli dosyaya kaydedilmesi.
 
**Hash Tablosu Özellikleri**

1. Hash tablosu open addressing yöntemiyle oluşturulmuştur.

2. Çakışma problemlerini çözmek için double hashing yöntemi kullanılmıştır.

3. Kullanılan hash fonksiyonları:

    h(key,i) = [h1(key) + i*h2(key)] mod M

    h1(key) = key mod M

    h2(key) = 1 + (key mod (M - 1))
    
    M: Sözlük boyutu

4. Hash tablosu case sensitive değildir. Örneğin "ARABA" ve "araba" aynı kelimedir.

5. Kelimeler sayıya çevrilirken Horner metodu kullanılmıştır.
 
**Örnek Girdi ve Çıktılar**

1. [Ekteki](/inputs/input1.txt) txt dosyası girdi olarak alınır.

![](/images/output-1.png)

2. Sözlükteki kelimeler ekrana yazdırılır.

![](/images/output-2.png)

3. Girilen bir kelimenin sözlükte olup olmadığı sorgulanır ve program sözlük hash.txt dosyasına kaydedilerek kapatılır.

![](/images/output-3.png)

4. Program tekrar başlatılıp hash.txt dosyasındaki tabloyu yeni bir hash tablosuna alır sonra yeni bir sözlük dosyası eklenir ve iki sözlük dosyasındada olan bir kelimenin sorgusu gerçekleştirilir.

![](/images/output-4.png)
