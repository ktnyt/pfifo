import _pfifo


class Reader(_pfifo.Reader):
    def __init__(self, name, mode=None):
        super(Reader, self).__init__(name, mode)

    def read(self):
        size = self.read_size()
        buf = b'\0' * size
        self.read_into(buf, size)
        return buf


class Writer(_pfifo.Writer):
    def __init__(self, name, mode=None):
        super(Writer, self).__init__(name, mode)
