%% Esempi di errore
figure(1)
img_nprocs = imread('errore_nprocs.png');
image(img_nprocs)
title('Errore: numero di thread non positivo');

figure(2)
img_dim_int = imread('errore_dim_non_intere.png');
image(img_dim_int)
title('Errore: le dimensioni devono essere valori interi');

figure(3)
img_dim_neg = imread('errore_dim_non_positive.png');
image(img_dim_neg)
title('Errore: le dimensioni dei dati di input non sono valori positivi');

figure(4)
img_dim_diverse = imread('errore_dim_non_coerenti.png');
image(img_dim_diverse)
title('Errore: le dimensioni non sono coerenti');


