%% ANALISI CONFIGURAZIONI

warning('OFF', 'MATLAB:table:ModifiedAndSavedVarnames');

tempi_ibrido = readtable('test_ibrido.xlsx', 'Range', 'E10:F14');
tempi = [table2array(tempi_ibrido(1,2)),table2array(tempi_ibrido(2,2)),table2array(tempi_ibrido(3,2)),table2array(tempi_ibrido(4,2))];
c = categorical({'16x1','8x2','4x4', '2x8'});
bar(c,tempi)
xlabel('Numero processi x Numero thread');
ylabel('Tempo di esecuzione (secondi)');
title('Analisi configurazioni con M=20000 ed N=15000')
