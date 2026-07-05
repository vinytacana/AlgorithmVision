package main;

/**
 * Contrato JNI entre a aplicacao Java (POO) e o motor grafico C++/OpenGL (CG).
 *
 * A camada Java NUNCA acessa a implementacao interna do motor: toda a
 * comunicacao acontece exclusivamente pelos metodos nativos declarados aqui.
 *
 * Threading: init() bloqueia a thread chamadora com o loop de renderizacao
 * (deve rodar em uma thread dedicada). Os demais metodos podem ser chamados
 * de qualquer thread (ex.: EDT do Swing) - o motor enfileira os comandos e
 * os aplica na thread de renderizacao, evitando corridas de dados.
 */
public class MotorGrafico {

    static {
        System.loadLibrary("motor");
    }

    /** Cria a janela OpenGL e executa o loop de renderizacao (bloqueante). */
    public native void init();

    /** Solicita o encerramento do loop de renderizacao. */
    public native void cleanup();

    /** Indica se o loop de renderizacao esta ativo. */
    public native boolean isRunning();

    // ----- Comandos (enfileirados para a thread de render) -----

    public native void setAlgorithm(int index);

    public native void setDistribution(int index);

    public native void setRenderMode(int index);

    public native void setArraySize(int n);

    /** Intervalo entre passos da ordenacao, em segundos (0.001 a 1.0). */
    public native void setTickRate(float secondsPerStep);

    public native void toggleSorting();

    public native void reset();

    // ----- Metricas (leitura de snapshots atomicos) -----

    public native long getComparisons();

    public native long getWrites();

    public native boolean isSorting();

    public native boolean hasFinished();

    // ----- Metadados (AlgorithmRegistry como fonte unica de verdade) -----

    public native int getAlgorithmCount();

    public native String getAlgorithmName(int index);

    public native String getAlgorithmDescription(int index);
}
