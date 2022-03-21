import shutil
import pathlib
import os
import typing
from abc import ABC, abstractmethod

from bzd.template.template import Template

class Factory(ABC):

    @abstractmethod
    def getName(self) -> str:
        """Give the name of the shell."""
        pass

    @abstractmethod
    def isCompatible(self) -> bool:
        """Tell if this is a compatible shell."""
        pass

    @abstractmethod
    def build(self, workspace: pathlib.Path) -> None:
        """Build the configuration."""
        pass

    @abstractmethod
    def install(self) -> None:
        """Install the shell."""
        pass

    @staticmethod
    def copyTree(source: pathlib.Path, destination: pathlib.Path, symlinks: bool = False) -> None:
        """Copy a files into a directory."""

        for item in os.listdir(source):
            s = source / item
            d = destination / item
            if s.is_dir():
                shutil.copytree(s, d, symlinks)
            else:
                shutil.copy2(s, d)

    @staticmethod
    def createEmptyDirectory(directory: pathlib.Path) -> None:
        """Create an empty directory."""

        # If the directory already exists, remove it.
        if directory.is_dir():
            shutil.rmtree(directory)
        directory.mkdir()

    @staticmethod
    def renderTemplate(templatePath: pathlib.Path, directoryMap: typing.Mapping[str, pathlib.Path]) -> str:
        """Render a template from files."""

        data = {}
        for name, path in directoryMap.items():
            sub = {}
            for fileName in os.listdir(path):
                sub[fileName] = (path / fileName).read_text()
            data[name] = sub

        template = Template.fromPath(templatePath)
        return template.render(data)
