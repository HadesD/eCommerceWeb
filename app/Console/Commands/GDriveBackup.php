<?php

namespace App\Console\Commands;

use Illuminate\Console\Command;

use Illuminate\Support\Facades\Log;

use App\Customize\LockerManager;
use App\Customize\LockerType;

class GDriveBackup extends Command
{
    /**
     * The name and signature of the console command.
     *
     * @var string
     */
    protected $signature = 'gdrive:backup';

    /**
     * The console command description.
     *
     * @var string
     */
    protected $description = 'Google Drive Backup';

    /**
     * Create a new command instance.
     *
     * @return void
     */
    public function __construct()
    {
        parent::__construct();
    }

    /**
     * Execute the console command.
     *
     * @return mixed
     */
    public function handle()
    {
        $flag = LockerManager::get(LockerType::GDRIVE_BACKUP);
        if ($flag && $flag->isLocking()) {
            return;
        }

        $flag->lock();

        try {
            $cmdLine = 'mysqldump \
                --routines \
                --single-transaction=TRUE \
                --skip-add-locks \
                --default-character-set=binary \
                --skip-extended-insert --quick -e \
                --column-statistics=0 \
                --host="%s" \
                --port="%s" \
                --user="%s" \
                --password="%s" \
                "%s" \
                | bzip2 -c \
                | gdrive -c ~/.gdrive upload \
                - --parent "%s" "%s.sql.bz2"
                ';

            exec(sprintf(
                $cmdLine,
                config('database.connections.' . config('database.default') . '.host'),
                config('database.connections.' . config('database.default') . '.port'),
                config('database.connections.' . config('database.default') . '.username'),
                config('database.connections.' . config('database.default') . '.password'),
                config('database.connections.' . config('database.default') . '.database'),
                config('app_custom.gdrive_backup_fid'),
                sprintf('%s_%s_%s', date('Y-m-d_H-i-s'), 'web', parse_url(config('app.url'))['host'])
            ));

            // Backup TLBBDB
            // foreach (\App\Server::all() as $server)
            // {
            //     exec(sprintf($cmdLine,
            //         $server->db_host,
            //         $server->db_port,
            //         $server->db_user,
            //         $server->db_pass,
            //         $server->db_name,
            //         $folderId,
            //         sprintf('%s_%s_%s', date('Y-m-d_H-i-s'), $server->name, env('SESSION_DOMAIN'))
            //     ));
            // }
        } catch (\Throwable $e) {
            Log::error($e->getMessage() . ':' . $e->getLine());
        }

        $flag->unlock();
    }
}
