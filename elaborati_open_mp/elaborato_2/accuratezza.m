%% FASE ACQUISIZIONE RISULTATI SCOPE

risultato_scope = xlsread('test_accuratezza_elab2_OpenMp.xlsx','C4:I4');
risultato_scope = risultato_scope'

%% ACQUISIZIONE RISULTATI IN MATLAB - 10^5 ELEMENTI
n = 8;
A = load('matrice.mat');
x = ones(1,n)';
risultato_matlab = A.A*x
errore_relativo = norm(risultato_matlab-risultato_scope)/norm(risultato_matlab)
