@extends('layouts.app')
@push('head.meta')
<title>{{ ($title ?? 'Bảng điều khiển') . ' | ' . config('app_custom.title') }}</title>
@endpush
@push('head.css')
    <link href="{{ mix('/css/dashboard/app.css') }}" rel="stylesheet" id="app-css" />
@endpush
@section('body.content')
    <div id="app"></div>
@endsection
@push('body.js')
    <script src="{{ mix('/js/dashboard/app.js') }}" id="app-script"></script>
@endpush
