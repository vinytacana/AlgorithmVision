package main.ui;

import main.controle.ControladorSimulacao;

import javax.swing.JPanel;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.awt.image.DataBufferInt;

/**
 * Superficie de exibicao dos quadros renderizados pelo motor grafico.
 *
 * HERANCA: estende {@link JPanel} para se comportar como qualquer componente
 * Swing. POLIMORFISMO: sobrescreve {@link #paintComponent(Graphics)} - o
 * Swing chama esse metodo pela referencia da superclasse sem saber que a
 * implementacao desenha um quadro vindo do OpenGL.
 *
 * O buffer de pixels da {@link BufferedImage} e compartilhado diretamente com
 * o motor (sem copias intermediarias): o controlador preenche o int[] e o
 * repaint() apenas desenha a imagem.
 */
public class PainelRenderizacao extends JPanel {

    private static final long serialVersionUID = 1L;

    private final transient ControladorSimulacao controlador;
    private transient BufferedImage quadro;
    private transient int[] pixels;

    public PainelRenderizacao(ControladorSimulacao controlador) {
        this.controlador = controlador;
        setPreferredSize(new Dimension(880, 700));
        setBackground(Color.WHITE);
        setOpaque(true);
    }

    /**
     * Pede um novo quadro ao motor (na EDT) e agenda o redesenho.
     * Chamado pelo laco de animacao da {@link JanelaPrincipal}.
     */
    public void atualizarQuadro() {
        final int largura = getWidth();
        final int altura = getHeight();
        if (largura <= 0 || altura <= 0) return;

        if (quadro == null || quadro.getWidth() != largura || quadro.getHeight() != altura) {
            quadro = new BufferedImage(largura, altura, BufferedImage.TYPE_INT_RGB);
            pixels = ((DataBufferInt) quadro.getRaster().getDataBuffer()).getData();
        }

        if (controlador.renderizar(largura, altura, pixels)) {
            repaint();
        }
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        if (quadro != null) {
            g.drawImage(quadro, 0, 0, null);
        }
    }
}
