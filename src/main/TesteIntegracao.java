package main;

import java.util.HashSet;
import java.util.Set;

/**
 * Teste de integracao da biblioteca nativa, sem interface grafica.
 *
 * Valida o caminho completo Java -> JNI -> OpenGL offscreen -> pixels:
 *   1. inicializa o motor (contexto oculto);
 *   2. renderiza um quadro e confere que ha conteudo (mais de uma cor);
 *   3. avanca a simulacao e confere que as metricas evoluem;
 *   4. libera os recursos.
 *
 * Uso: java -Djava.library.path=. -cp <classes> main.TesteIntegracao
 */
public final class TesteIntegracao {

    private TesteIntegracao() {
    }

    public static void main(String[] args) {
        final MotorGrafico motor = new MotorGrafico();

        if (!motor.init()) {
            System.err.println("FALHA: init() retornou false");
            System.exit(1);
        }

        motor.setArraySize(50);

        final int largura = 320;
        final int altura = 240;
        final int[] pixels = new int[largura * altura];
        if (!motor.renderFrame(largura, altura, pixels)) {
            System.err.println("FALHA: renderFrame() retornou false");
            System.exit(1);
        }

        final Set<Integer> coresDistintas = new HashSet<>();
        for (int pixel : pixels) coresDistintas.add(pixel);
        if (coresDistintas.size() < 2) {
            System.err.println("FALHA: quadro uniforme (nada foi desenhado)");
            System.exit(1);
        }

        motor.step(); // parado: nao deve avancar nem quebrar
        final long comparacoesAntes = motor.getComparisons();
        motor.toggleSorting();
        motor.step();
        motor.step();
        final long comparacoesDepois = motor.getComparisons();
        if (comparacoesDepois <= comparacoesAntes) {
            System.err.println("FALHA: metricas nao evoluiram apos step()");
            System.exit(1);
        }

        // Segundo quadro em outro tamanho (recria o framebuffer offscreen).
        final int[] pixels2 = new int[640 * 480];
        if (!motor.renderFrame(640, 480, pixels2)) {
            System.err.println("FALHA: renderFrame() em novo tamanho");
            System.exit(1);
        }

        motor.cleanup();

        System.out.println("OK: quadro 320x240 com " + coresDistintas.size()
                + " cores distintas; comparacoes " + comparacoesAntes
                + " -> " + comparacoesDepois + "; redimensionamento OK.");
    }
}
