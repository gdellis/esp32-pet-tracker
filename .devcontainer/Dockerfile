FROM espressif/idf:v5.3.1

ENV HOME=/root
WORKDIR /workspace

RUN useradd -m -s /bin/bash devuser && \
    chown -R devuser:devuser /workspace

USER devuser

RUN /opt/esp/python_env/idf5.3_py3.10_env/bin/python -m pip install --no-cache-dir \
    pygdbmi==0.9.0.0 \
    python-magic==0.4.27 \
    websocket-client==1.8.0 \
    construct==2.10.68 \
    bitstring==4.2.3 \
    ecdsa==0.19.0

RUN echo 'source /opt/esp/idf/export.sh' >> ~/.bashrc
