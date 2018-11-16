%% FASE ACQUISIZIONE RISULTATI SCOPE

risultati_scope = xlsread('test_accuratezza_integrale.xlsx','B2:F2');

%% CALCOLO VALORE INTEGRALE 

F = @(x) x./(x.^2+5) 
risultato_matlab = quad(F,0,1)

%% CONFRONTO RISULTATI IN MATLAB - 10^4 ELEMENTI

err_rel_10k = abs(risultati_scope(1)-risultato_matlab)/abs(risultato_matlab)

%% CONFRONTO RISULTATI IN MATLAB - 10^5 ELEMENTI

err_rel_100k = abs(risultati_scope(2)-risultato_matlab)/abs(risultato_matlab)

%% CONFRONTO RISULTATI IN MATLAB - 10^6 ELEMENTI

err_rel_1M = abs(risultati_scope(3)-risultato_matlab)/abs(risultato_matlab)

%% CONFRONTO RISULTATI IN MATLAB - 10^7 ELEMENTI

err_rel_10M = abs(risultati_scope(4)-risultato_matlab)/abs(risultato_matlab)

%% ACQUISIZIONE RISULTATI IN MATLAB - 10^8 ELEMENTI

err_rel_100M = abs(risultati_scope(5)-risultato_matlab)/abs(risultato_matlab)
