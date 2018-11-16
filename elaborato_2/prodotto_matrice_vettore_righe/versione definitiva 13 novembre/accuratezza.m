%% FASE ACQUISIZIONE RISULTATI SCOPE

risultato_scope = xlsread('test_accuratezza_el2_prodotto_righe.xlsx','C4:I4');


%% ACQUISIZIONE RISULTATI IN MATLAB - 10^5 ELEMENTI
m = 7;
n = 8;
i = (1:m);
A = ones(m,n)*pi;
x = ones(n,1);
risultato_matlab = A*x;
errore_relativo = norm(risultato_matlab-risultato_scope)/norm(risultato_matlab)
