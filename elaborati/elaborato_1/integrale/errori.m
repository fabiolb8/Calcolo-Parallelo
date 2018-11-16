%% Esempi di errore

figure(1)
img_nprocs = imread('errore_nprocs.png');
image(img_nprocs)
title('Errore: numero di processori non positivo');

figure(2)
img_dim_neg = imread('errore_num_intervalli_non_intero.png');
image(img_dim_neg)
title('Errore: numero di intervalli non � un intero');

figure(3)
img_dim_neg = imread('errore_numIntervalli_negativo.png');
image(img_dim_neg)
title('Errore: numero di intervalli non � positivo');

figure(4)
img_dim_nprocs = imread('errore_bMinoreDia.png');
image(img_dim_nprocs)
title('Errore: intervallo [a,b] non coerente');

figure(5)
img_dim_diverse = imread('errore_Intevalli_minore_Nprocs.png');
image(img_dim_diverse)
title('Errore: numero di intervalli � minore del numero di processori');
