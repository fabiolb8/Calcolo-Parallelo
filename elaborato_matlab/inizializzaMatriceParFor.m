function A = inizializzaMatriceParFor(N,M,elemento)

    parfor i=1:N
        for j=1:M
            A(i,j) = elemento+i-j;
        end
    end

end

