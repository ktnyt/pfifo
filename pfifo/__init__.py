import _pfifo


class Reader(_pfifo.Reader):
    def __init__(self, name, mode=None):
        super(Reader, self).__init__(name, mode)


class Writer(_pfifo.Writer):
    def __init__(self, name, mode=None):
        super(Writer, self).__init__(name, mode)
