package main.controle;

/**
 * Abstracao da simulacao de ordenacao consumida pela interface grafica.
 *
 * A camada de visao (main.ui) depende APENAS desta interface - nunca da
 * implementacao nativa. Isso e o Principio da Inversao de Dependencia em
 * pratica: e possivel trocar a implementacao (motor JNI real, um mock para
 * testes, uma versao 100% Java...) sem alterar uma linha da UI, gracas ao
 * polimorfismo.
 */
public interface ControladorSimulacao {

    // ----- Ciclo de vida -----

    /** Prepara os recursos da simulacao. @return false se falhou. */
    boolean inicializar();

    /** Libera os recursos da simulacao. */
    void encerrar();

    // ----- Controle de execucao -----

    void iniciarOuPausar();

    void reiniciar();

    /**
     * Avanca a simulacao se o intervalo configurado ja passou.
     * Deve ser chamada periodicamente pelo laco de animacao da UI.
     */
    void avancarSeNecessario();

    /**
     * Preenche {@code destino} (ARGB, largura*altura) com o quadro atual.
     *
     * @return true se o quadro foi renderizado.
     */
    boolean renderizar(int largura, int altura, int[] destino);

    // ----- Configuracao -----

    void selecionarAlgoritmo(int indice);

    void selecionarDistribuicao(int indice);

    void selecionarVisualizacao(int indice);

    void definirTamanho(int n);

    /** Intervalo entre passos, em segundos (ex.: 0.03). */
    void definirVelocidade(float segundosPorPasso);

    // ----- Estado e metricas -----

    String obterStatus();

    boolean estaExecutando();

    long obterComparacoes();

    long obterAcessos();

    // ----- Metadados dos algoritmos -----

    int quantidadeAlgoritmos();

    String nomeAlgoritmo(int indice);

    String descricaoAlgoritmo(int indice);
}
