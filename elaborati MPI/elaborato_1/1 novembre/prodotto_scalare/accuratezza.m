%% FASE ACQUISIZIONE RISULTATI SCOPE

risultati_scope = xlsread('test_accuratezza_prodotto_scalare.xlsx','C4:G4');


%% ACQUISIZIONE RISULTATI IN MATLAB - 10^5 ELEMENTI

x(1:100000) = pi;
y(1:100000) = 1;
risultato_scope = risultati_scope(2)
risultato_matlab = dot(x,y)
errore_relativo = abs(risultato_scope-risultato_matlab)/abs(risultato_matlab)
