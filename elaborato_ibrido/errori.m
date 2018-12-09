%% Esempi di errore
figure(1)
img_nprocs = imread('errore_nprocs.png');
image(img_nprocs)
title('Errore: numero di processi non positivo');

figure(2)
img_dim_int = imread('errore_dim_non_intere.png');
image(img_dim_int)
title('Errore: le dimensioni devono essere valori interi');

figure(3)
img_dim_neg = imread('errore_dim_non_positive.png');
image(img_dim_neg)
title('Errore: le dimensioni dei dati di input non sono valori positivi');

figure(4)
img_dim_nprocs = imread('errore_M_minore_nprocs.png');
image(img_dim_nprocs)
title('Errore: la dimensione di riga della matrice è inferiore al numero di processi');

figure(5)
img_dim_diverse = imread('errore_dim_non_coerenti.png');
image(img_dim_diverse)
title('Errore: le dimensioni non sono coerenti');

figure(6)
img_nprocs = imread('errore_nthread.png');
image(img_nprocs)
title('Errore: numero di thread non positivo');

figure(7)
img_nprocs = imread('errore_numcore.png');
image(img_nprocs)
title('Errore: il numero totale di core deve essere pari a 16');

