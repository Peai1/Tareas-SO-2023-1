JC = javac
.SUFFIXES: .java .class

# Definir los archivos .java a compilar
CLASSES = \
	Main.java

# Regla predeterminada para compilar
default: classes

# Regla para compilar las clases
classes:
	$(JC) $(CLASSES)

# Regla para limpiar los archivos .class
clean:
	$(RM) *.class
