%% Esempi di errore

figure(1)
img_nprocs = imread('errore_nprocs.png');
image(img_nprocs)
title('Errore: numero di processori non positivo');

figure(2)
img_dim_neg = imread('errore_dim_non_intero.png');
image(img_dim_neg)
title('Errore: la dimensione dei vettori non è un intero');

figure(3)
img_dim_neg = imread('errore_dim_nonPositiva.png');
image(img_dim_neg)
title('Errore: la dimensione dei vettori non è positiva');

figure(4)
img_dim_nprocs = imread('errore_dim_incoerente_con_nprocs.png');
image(img_dim_nprocs)
title('Errore: la dimensione dei vettori è minore del numero di processori');

figure(5)
img_dim_diverse = imread('errore_dim_diverse.png');
image(img_dim_diverse)
title('Errore: le dimensione dei vettori sono diverse');
