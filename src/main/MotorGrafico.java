package main;

/**
 * Contrato JNI da biblioteca nativa de renderizacao (motor.dll / libmotor.so).
 *
 * O motor e uma BIBLIOTECA PASSIVA: nao cria janela visivel nem possui loop
 * proprio. A aplicacao Java e a dona do ciclo de vida - ela cria a janela
 * Swing, controla o tempo da simulacao e pede quadros renderizados via
 * {@link #renderFrame(int, int, int[])}, que devolve os pixels (ARGB) do
 * quadro desenhado pelo pipeline OpenGL em um framebuffer offscreen.
 *
 * Contrato de threading: TODAS as chamadas devem ocorrer na MESMA thread,
 * pois o contexto OpenGL pertence a thread que chamou {@link #init()}.
 * Na aplicacao Swing, essa thread e a Event Dispatch Thread (EDT).
 *
 * Esta classe e o UNICO ponto de contato entre Java e C++ (encapsulamento):
 * o restante da aplicacao depende apenas da interface
 * {@code main.controle.ControladorSimulacao}.
 */
public final class MotorGrafico {

    static {
        System.loadLibrary("motor");
    }

    // ----- Ciclo de vida -----

    /** Cria o contexto OpenGL oculto e os recursos de renderizacao. */
    public native boolean init();

    /** Libera todos os recursos graficos. */
    public native void cleanup();

    // ----- Simulacao (o Java decide QUANDO avancar) -----

    /** Avanca um passo se a ordenacao estiver ativa ou finalizando. */
    public native void step();

    public native void reset();

    public native void toggleSorting();

    public native boolean isSorting();

    public native boolean hasFinished();

    // ----- Configuracao -----

    public native void setAlgorithm(int index);

    public native void setDistribution(int index);

    public native void setRenderMode(int index);

    public native void setArraySize(int n);

    // ----- Metricas -----

    public native long getComparisons();

    public native long getWrites();

    // ----- Metadados (AlgorithmRegistry como fonte unica de verdade) -----

    public native int getAlgorithmCount();

    public native String getAlgorithmName(int index);

    public native String getAlgorithmDescription(int index);

    // ----- Renderizacao offscreen -----

    /**
     * Renderiza o estado atual em um framebuffer offscreen de largura x altura
     * e preenche {@code pixelsOut} (formato ARGB, tamanho >= largura*altura).
     *
     * @return true se o quadro foi renderizado com sucesso.
     */
    public native boolean renderFrame(int width, int height, int[] pixelsOut);
}
