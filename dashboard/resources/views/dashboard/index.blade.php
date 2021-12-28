@extends('layouts.app')
@push('head.meta')
<title>{{ ($title ?? 'Bảng điều khiển') . ' | ' . config('app_custom.title') }}</title>
@endpush
@push('head.css')
    <link href="{{ mix('/assets/css/app.css') }}" rel="stylesheet" id="app-css" />
@endpush
@section('body.content')
    <div id="app"></div>
@endsection
@push('body.js')
    <script src="{{ mix('/assets/js/app.js') }}" id="app-script"></script>
@endpush
