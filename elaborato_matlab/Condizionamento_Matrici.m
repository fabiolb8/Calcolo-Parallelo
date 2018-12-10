%% Calcolo indice di condizionamento di matrici quadrate in sequenziale e in parallelo
m=100;
g=@()condFor(m);
h=@()condParFor(m);
tempo_for=timeit(g)
tempo_parFor=timeit(h)