%% Esempi di errore
figure(1)
img_nprocs = imread('errore_nprocs.png');
image(img_nprocs)
title('Errore: numero di processori non positivo');

figure(2)
img_dim_int = imread('errore_dim_non_intere.png');
image(img_dim_int)
title('Errore: le dimensioni devono essere valori interi');

figure(3)
img_dim_neg = imread('errore_dim_non_positive.png');
image(img_dim_neg)
title('Errore: le dimensioni dei dati di input non sono valori positivi');

figure(4)
img_dim_nprocs = imread('errore_m_n_minore_q.png');
image(img_dim_nprocs)
title('Errore: la dimensione di riga o di colonna della matrice è inferiore a q');

figure(5)
img_dim_diverse = imread('errore_dim_non_coerenti.png');
image(img_dim_diverse)
title('Errore: le dimensioni non sono coerenti');

figure(6)
img_dim_nprocs = imread('errore_q_non_intero.png');
image(img_dim_nprocs)
title('Errore: la dimensione della griglia non è un valore intero');

figure(7)
img_dim_diverse = imread('errore_nprocs_non_quadrato_perf_di_q.png');
image(img_dim_diverse)
title('Errore: il numero di processi non è un quadrato perfetto');


