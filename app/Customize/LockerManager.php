<?php

namespace App\Customize;

class LockerType
{
    const CHECK_PAYMENT = 'checking_payment';
    const GDRIVE_BACKUP = 'gdrive_backup';
}

class LockerManager
{
    const FILE_DIR_PATH = __DIR__ . '/../../storage/app/flags';
    const FILE_EXT = 'locker';

    const STATE_UNLOCK = 0;
    const STATE_LOCK = 1;

    private $filePath;

    public static function get(string $lockerName) : self
    {
        $self = new self;
        $self->filePath = sprintf('%s/%s.%s', self::FILE_DIR_PATH, $lockerName, self::FILE_EXT);
        if (!file_exists($self->filePath))
        {
            return $self->setState(self::STATE_UNLOCK) ? $self : null;
        }

        return $self;
    }

    public function lock() : void
    {
        $this->setState(self::STATE_LOCK);
    }

    public function unlock() : void
    {
        $this->setState(self::STATE_UNLOCK);
    }

    public function isLocking() : bool
    {
        return $this->getState() === self::STATE_LOCK;
    }

    private function getState() : int
    {
        $hFile = fopen($this->filePath, 'rb');
        return $hFile ? intval(fgets($hFile)) : self::STATE_UNLOCK;
    }

    private function setState(int $state) : bool
    {
        $hFile = fopen($this->filePath, 'wb');

        if ($hFile !== false)
        {
            fwrite($hFile, $state);
            fclose($hFile);

            return true;
        }

        return false;
    }
}

