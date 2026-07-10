package main.controle;

import main.MotorGrafico;

/**
 * Implementacao de {@link ControladorSimulacao} baseada no motor nativo JNI.
 *
 * Encapsula completamente a classe {@link MotorGrafico}: a UI nunca sabe que
 * existe codigo nativo por tras. Tambem e responsavel pelo RITMO da simulacao
 * (o motor so avanca quando {@link #avancarSeNecessario()} decide), ja que o
 * modelo "biblioteca passiva" deixa o controle de tempo inteiramente no Java.
 */
public class ControladorMotorNativo implements ControladorSimulacao {

    private static final float VELOCIDADE_MINIMA = 0.001f;
    private static final float VELOCIDADE_MAXIMA = 1.0f;

    private final MotorGrafico motor = new MotorGrafico();

    private float segundosPorPasso = 0.03f;
    private long ultimoPassoNanos = System.nanoTime();

    @Override
    public boolean inicializar() {
        return motor.init();
    }

    @Override
    public void encerrar() {
        motor.cleanup();
    }

    @Override
    public void iniciarOuPausar() {
        motor.toggleSorting();
    }

    @Override
    public void reiniciar() {
        motor.reset();
    }

    @Override
    public void avancarSeNecessario() {
        final long agora = System.nanoTime();
        final long intervalo = (long) (segundosPorPasso * 1_000_000_000L);
        if (agora - ultimoPassoNanos >= intervalo) {
            motor.step();
            ultimoPassoNanos = agora;
        }
    }

    @Override
    public boolean renderizar(int largura, int altura, int[] destino) {
        return motor.renderFrame(largura, altura, destino);
    }

    @Override
    public void selecionarAlgoritmo(int indice) {
        motor.setAlgorithm(indice);
    }

    @Override
    public void selecionarDistribuicao(int indice) {
        motor.setDistribution(indice);
    }

    @Override
    public void selecionarVisualizacao(int indice) {
        motor.setRenderMode(indice);
    }

    @Override
    public void definirTamanho(int n) {
        motor.setArraySize(n);
    }

    @Override
    public void definirVelocidade(float segundosPorPasso) {
        this.segundosPorPasso =
                Math.min(VELOCIDADE_MAXIMA, Math.max(VELOCIDADE_MINIMA, segundosPorPasso));
    }

    @Override
    public String obterStatus() {
        if (motor.hasFinished()) return "Finalizado";
        return motor.isSorting() ? "Executando" : "Pronto";
    }

    @Override
    public boolean estaExecutando() {
        return motor.isSorting();
    }

    @Override
    public long obterComparacoes() {
        return motor.getComparisons();
    }

    @Override
    public long obterAcessos() {
        return motor.getWrites();
    }

    @Override
    public int quantidadeAlgoritmos() {
        return motor.getAlgorithmCount();
    }

    @Override
    public String nomeAlgoritmo(int indice) {
        return motor.getAlgorithmName(indice);
    }

    @Override
    public String descricaoAlgoritmo(int indice) {
        return motor.getAlgorithmDescription(indice);
    }
}
