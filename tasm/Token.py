class Token:
    __slots__ = ('type', 'value')

    def __init__(self, type, value):
        self.type = type
        self.value = value
