#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 100 // Maksimum dugum sayisini tanimlar

// Kenar yapisi, bir dugum ve kenar agirligini içerir
struct Edge {
    int node; // Baglanilan dugum
    int weight; // Kenar agirligi
    struct Edge* next; // Sonraki kenar
};

// Grafik yapisi, dugum sayisi ve komsuluk listelerini icerir
struct Graph {
    int num_nodes; // Graftaki toplam dugum sayisi
    struct Edge* adjacency_list[MAX_NODES]; // Her dugumun komsuluk listesini tutan diziler
};

// @brief Grafigi baslatan fonksiyon
// @param graph grafi alir.
// @param N dugum sayisi
void initGraph(struct Graph* graph, int N) {
    int i;
    graph->num_nodes = N; // Dugum sayisini ayarla
    for (i = 0; i < N; i++) {
        graph->adjacency_list[i] = NULL; // Her dugum icin komsuluk listesini baslat
    }
}

// @brief Grafige kenar ekleyen fonksiyon
// @param graph graf
// @param src kaynak dugumu 
// @param dest hedef dugumu 
// @param weight kenar agirligi
void addEdge(struct Graph* graph, char src, char dest, int weight) {
    int src_index = src - 'A'; // Kaynak dugumun indeksini hesapla
    int dest_index = dest - 'A'; // Hedef dugumun indeksini hesapla

    // Kaynak dugume kenar ekleme
    struct Edge* new_edge = (struct Edge*)malloc(sizeof(struct Edge));
    new_edge->node = dest_index; // Hedef dugumu ayarla
    new_edge->weight = weight; // Kenar agirligini ayarla
    new_edge->next = graph->adjacency_list[src_index]; // Yeni kenari mevcut listenin basina ekle
    graph->adjacency_list[src_index] = new_edge; // Listeyi güncelle

    // Hedef dugume kenar ekleme (çift yönlü)
    new_edge = (struct Edge*)malloc(sizeof(struct Edge));
    new_edge->node = src_index; // Kaynak dugumu ayarla
    new_edge->weight = weight; // Kenar agirligini ayarla
    new_edge->next = graph->adjacency_list[dest_index]; // Yeni kenari mevcut listenin basina ekle
    graph->adjacency_list[dest_index] = new_edge; // Listeyi güncelle
}

// @ brief Dosyadan grafigi okuyan fonksiyon
// @param graph graf
// @param M kenar sayisi
// @param filename dosya ismi 
void readFile(struct Graph* graph, int M, char* filename) {
    int i;
    FILE* fs = fopen(filename, "r"); // Dosyayi ac
    if (fs == NULL) { // Dosya acilamadiysa hata ver
        printf("Dosya yok.\n");
        exit(1);
    }

    for (i = 0; i < M; ++i) { // Her kenar icin
        char src, dest;
        int weight;
        fscanf(fs, " %c %c %d", &src, &dest, &weight); // Dosyadan kenar bilgilerini oku
        addEdge(graph, src, dest, weight); // Kenari grafige ekle
    }
    fclose(fs); // Dosyayi kapat
}

// @brief Komsuluk listesini yazdiran fonksiyon
// @param graph graf
// @param node dugum
void printAdjacencyList(struct Graph* graph, char node) {
    int index = node - 'A'; // Dugumun indeksini hesapla
    struct Edge* current = graph->adjacency_list[index]; // Dugumun komsuluk listesini al
    printf("%c dugumunün komþuluk listesi:\n", node);
    while (current != NULL) { // Komsuluk listesini dolas
        printf("Düðüm: %c, Aðýrlýk: %d\n", current->node + 'A', current->weight);
        current = current->next; // Sonraki kenara gec
    }
}

// @brief DFS yardimci fonksiyonu, donguleri bulur
// @param graph graf
// @param node o anda bulundugum dugum
// @param visited o dugumu ne zaman ziyaret ettigimi tutuyor
// @param parent ebeveyn dugumu tutuyor. Hangi dugumden geldigimi tutabilmem lazým.Undirected graph oldugu icin knedine hemen donunce cycle olustu dememesi lazim.
// @param path sekil olusunce o seklin hangi dugumleri gezerek olustugunu yani yolu tutuyor.
// @param shapes_count toplam sekil sayisini tutuyor
// @param shape_perimeter sekillerin cevrelerini tutuyor
// @param shapes bu matris sekil sayisi kadar satir, 100 uzunlugunda sutundan olusuyor. 1.satirda 1.sekli tutuyor. sutunlarda ise onun yolunu tutuyor.
// @param shape_lengths bu dizi sekil tipini tutuyor. 1.elemani ilk seklin kacgen oldugunu tutuyor mesela.
void DFSUtil(struct Graph* graph, int node, int* visited, int parent, int depth, int* path, int* shapes_count, int* shape_perimeter, int** shapes, int* shape_lengths) {
    int i;
    visited[node] = depth; // Dugumu ziyaret edildi olarak isaretle
    path[depth] = node; // Dugumu yola ekle

    struct Edge* current_edge = graph->adjacency_list[node]; // Mevcut dugumun kenarlarini al
    while (current_edge != NULL) { // Kenarlarý dolas
        int neighbor = current_edge->node; // Komsu dugumu al

        if (neighbor != parent) { // Eger komsu dugum ebeveyn dugum degilse
            if (visited[neighbor] == -1) { // Eger komsu dugum ziyaret edilmediyse
                DFSUtil(graph, neighbor, visited, node, depth + 1, path, shapes_count, shape_perimeter, shapes, shape_lengths); // DFS ile devam et
            } else if (visited[neighbor] < visited[node]) { // Eger komsu dugum zaten ziyaret edildiyse ve bir dongu olusturuyorsa
                int length = depth - visited[neighbor] + 1; // Dongu uzunlugunu hesapla
                shape_lengths[*shapes_count] = length; // Dongu uzunlugunu kaydet
                shapes[*shapes_count] = (int*)malloc(length * sizeof(int)); // Dongu icin hafiza ayir
                int perimeter = 0; // Dongu cevresini hesapla
                for (i = visited[neighbor]; i <= depth; i++) {
                    shapes[*shapes_count][i - visited[neighbor]] = path[i];
                    if (i < depth) {
                        struct Edge* e = graph->adjacency_list[path[i]];
                        while (e->node != path[i + 1]) e = e->next;
                        perimeter += e->weight;
                    } else {
                        struct Edge* e = graph->adjacency_list[path[i]];
                        while (e->node != path[visited[neighbor]]) e = e->next;
                        perimeter += e->weight;
                    }
                }
                shape_perimeter[*shapes_count] = perimeter; // Dongu cevresini kaydet
                (*shapes_count)++; // Benzersizse dongu sayisini artir
            }
        }
        current_edge = current_edge->next; // Sonraki kenara gec
    }
    visited[node] = -1; // Dugumu tekrar ziyaret edilmedi olarak isaretle
}

// @brief Sekilleri bulan fonksiyon
// @param graph graf
// @param shapes tum sekilleri ve yolunu tutan matris
// @param shape_lengths bu dizi sekil tipini tutuyor.
// @param shape_perimeter sekillerin cevrelerini tutuyor
// @param shapes_count toplam sekil sayisini tutuyor
void findShapes(struct Graph* graph, int** shapes, int* shape_lengths, int* shape_perimeter, int* shape_count) {
    int i;
    int visited[MAX_NODES]; // Dugumlerin kacinci sirada gezildigini tutan dizi
    int path[MAX_NODES]; // DFS sirasinda izlenen yolu tutan dizi
    memset(visited, -1, sizeof(visited)); // Ziyaret dizisini -1 ile baþlat

    *shape_count = 0; // Sekil sayisini sýfýrla
    for (i = 0; i < graph->num_nodes; i++) { // Her dugum icin
        if (visited[i] == -1) { // Eger dugum ziyaret edilmediyse
            DFSUtil(graph, i, visited, -1, 0, path, shape_count, shape_perimeter, shapes, shape_lengths); // DFS baþlat
        }
    }
}

// @brief Yinelenen sekilleri kaldiran fonksiyon
// @param shapes tum sekilleri ve yolunu tutan matris
// @param shape_lengths bu dizi sekil tipini tutuyor.
// @param shape_perimeter sekillerin cevrelerini tutuyor
// @param shapes_count toplam sekil sayisini tutuyor
// @param shapetype bu matrisin ilk sutunu sekil tipini 2.sutunu o sekilden kac tane oldugunu tutuyor.
// @param N kenar sayisi
void removeSameShapes(int** shapes, int* shape_lengths, int* shape_perimeter, int* shape_count, int*** shapetype, int N) {
    int i, j, k, m, counter = 0;
    for (i = 0; i < *shape_count; i++) {
        for (j = i + 1; j < *shape_count; j++) {
            if (shape_lengths[i] == shape_lengths[j]) { // Eger iki sekil ayni uzunluktaysa
                for (k = 0; k < shape_lengths[i]; k++) {
                    for (m = 0; m < shape_lengths[j]; m++) {
                        if (shapes[i][k] == shapes[j][m]) {
                            counter++;
                        }
                    }
                }
                if (counter == shape_lengths[i]) { // Eger sekiller ayniysa
                    free(shapes[j]); // Sekli serbest birak
                    for (m = j; m < *shape_count - 1; m++) { // Sekilleri sola kaydir
                        shapes[m] = shapes[m + 1];
                        shape_lengths[m] = shape_lengths[m + 1];
                        shape_perimeter[m] = shape_perimeter[m + 1];
                    }
                    j--; // Bir sonraki sekli kontrol etmek icin donguyu geri al
                    (*shape_count)--; // Toplam sekil sayisini azalt
                    for (m = 0; m < N - 2; m++) {
                        if (shape_lengths[i] == (*shapetype)[m][0]) {
                            (*shapetype)[m][1]--;
                        }
                    }
                }
            }
            counter = 0;
        }
    }
}

// @brief Sekiller tipine gore siralanir. En kucukten olan sekilden yani ucgenden baslayarak siraliyor.
// @param shapes tum sekilleri ve yolunu tutan matris
// @param shape_lengths bu dizi sekil tipini tutuyor.
// @param shape_perimeter sekillerin cevrelerini tutuyor
// @param shapes_count toplam sekil sayisini tutuyor
void sortShapes(int** shapes, int* shape_lengths, int* shape_perimeter, int shape_count) {
    int i, j;
    for (i = 0; i < shape_count - 1; i++) {
        for (j = 0; j < shape_count - i - 1; j++) {
            if (shape_lengths[j] > shape_lengths[j + 1]) {
                int* temp_shape = shapes[j];
                shapes[j] = shapes[j + 1];
                shapes[j + 1] = temp_shape;

                int temp_length = shape_lengths[j];
                shape_lengths[j] = shape_lengths[j + 1];
                shape_lengths[j + 1] = temp_length;

                int temp_perimeter = shape_perimeter[j];
                shape_perimeter[j] = shape_perimeter[j + 1];
                shape_perimeter[j + 1] = temp_perimeter;
            }
        }
    }
}

// @brief Sekil bilgilerini yazdiran fonksiyon
// @param shapes tum sekilleri ve yolunu tutan matris
// @param shape_lengths bu dizi sekil tipini tutuyor.
// @param shape_perimeter sekillerin cevrelerini tutuyor
// @param shapes_count toplam sekil sayisini tutuyor
// @param N kenar sayisi
void printShapeInfo(int** shapes, int* shape_lengths, int* shape_perimeter, int shape_count, int N) {
    int i, j, **shapetype, k,typeCount=0;
    shapetype = (int**)malloc((N - 2) * sizeof(int*)); 
    for (i = 0; i < N - 2; i++) {
        shapetype[i] = (int*)malloc(2 * sizeof(int));
        shapetype[i][0] = i + 3; // Seklin Tipi
        shapetype[i][1] = 0; // Sayisi
    }

	//Bu dongulerde kac sekilden kac tane oldugunu hesapliyor.
    for (i = 0; i < shape_count; i++) {
        for (j = 0; j < N - 2; j++) {
            if (shape_lengths[i] == shapetype[j][0]) {
                shapetype[j][1]++;
            }
        }
    }
	removeSameShapes(shapes, shape_lengths, shape_perimeter, &shape_count, &shapetype, N);

	sortShapes(shapes, shape_lengths, shape_perimeter, shape_count); // Siralama iþlemi
    
    
    printf("Sekil Sayisi: %d\n", shape_count);
    for (i = 0; i < N - 2; i++) {
        printf("%d'gen sayisi: %d\n", shapetype[i][0], shapetype[i][1]);
    }
    for (i = 0; i < shape_count; i++) {
    	typeCount++;
        for (j = 0; j < N - 2; j++) {
            if (shape_lengths[i] == shapetype[j][0]) {
            	if(shape_lengths[i]!=shape_lengths[i-1]){
            		typeCount=0;
            		printf("\n");
				}
                printf("%d.%d'gen: ", typeCount+1, shapetype[j][0]);
            }
        }
        for (j = 0; j < shape_lengths[i]; j++) {
            printf("%c ", shapes[i][j] + 'A');
        }
        printf("%c Cevre: %d\n", shapes[i][0] + 'A', shape_perimeter[i]);
    }
    for (i = 0; i < N - 2; i++) {
        free(shapetype[i]);
    }
    free(shapetype);
}
int main() {
    struct Graph graph;
    int N, M; // Dugum ve kenar sayisi
    char filename[50]; // Dosya adi
    int i;

    printf("Dosya ismini giriniz.\n");
    scanf("%s", filename);

    printf("Dugum sayisini giriniz.\n");
    scanf("%d", &N);

    printf("Kenar sayisini giriniz.\n");
    scanf("%d", &M);

    initGraph(&graph, N); // Grafi baslat
    readFile(&graph, M, filename); // Grafi dosyadan oku

    int* shapes[MAX_NODES]; // Sekillerin tutuldugu matrisi
    int shape_lengths[MAX_NODES] = {0}; // Sekil kenar sayisi
    int shape_perimeter[MAX_NODES] = {0}; // Sekil cevreleri
    int shape_count = 0; // Sekil sayisi

    findShapes(&graph, shapes, shape_lengths, shape_perimeter, &shape_count); // Sekilleri bul
    printShapeInfo(shapes, shape_lengths, shape_perimeter, shape_count, N); // Sekil bilgilerini yazdir

    for (i = 0; i < shape_count; i++) {
        free(shapes[i]); // Sekil hafizasini serbest birak
    }

    return 0;
}

