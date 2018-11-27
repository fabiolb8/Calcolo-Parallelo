%% CASO M<N

warning('OFF', 'MATLAB:table:ModifiedAndSavedVarnames');

tabella_efficienza_righe = readtable('test_valutazione_elab2_prodotto_righe.xlsx', 'Range', 'C38:E39');
tabella_efficienza_colonne = readtable('test_valutazione_elab2_prodotto_colonne.xlsx', 'Range', 'C38:E39');
tabella_efficienza_blocchi = readtable('test_valutazione_elab2_prodotto_blocchi.xlsx', 'Range', 'C32:E33');

righe = [table2array(tabella_efficienza_righe(1,1)),table2array(tabella_efficienza_righe(1,2)),table2array(tabella_efficienza_righe(1,3))];
colonne = [table2array(tabella_efficienza_colonne(1,1)),table2array(tabella_efficienza_colonne(1,2)),table2array(tabella_efficienza_colonne(1,3))];
blocchi = [table2array(tabella_efficienza_blocchi(1,1)),table2array(tabella_efficienza_blocchi(1,2)),table2array(tabella_efficienza_blocchi(1,3))];

c = categorical({'5000','50000','500000 / 500000 / 300000'});
y = [righe(1) colonne(1) blocchi(1); righe(2) colonne(2) blocchi(2); righe(3) colonne(3) blocchi(3)];
bar(c,y)
xlabel('Dimensioni della matrice');
ylabel('Efficienza E(p)');
ylim([0 1.2]);
title('Analisi comparata con griglia 2x2 con M=1000 < N')
legend('Righe','Colonne','Blocchi','Location','NorthWest')