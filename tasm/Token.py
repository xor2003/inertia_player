class Token:
    __slots__ = ('type', 'value')

    def __init__(self, type, value):
        self.type = type
        self.value = value

    def __str__(self):
        return "Token(%s, %s)" % (self.type, self.value)

    def __repr__(self):
        return "Token(%s, %s)" % (self.type, self.value)
