function A = inizializzaMatriceFor(N,M,elemento)

    for i=1:N
        for j=1:M
            A(i,j) = elemento+i-j;
        end
    end

end

